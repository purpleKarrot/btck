// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iterator>
#include <memory>
#include <new>
#include <optional>
#include <span>
#include <stdexcept>
#include <string_view>
#include <utility>

#include <btck/btck.h>

namespace btck::detail {

template <typename T>
struct arrow_proxy
{
  auto operator->() -> T* { return &r; }
  T r;
};

template <typename T>
arrow_proxy(T) -> arrow_proxy<T>;

template <typename Range>
class range_iterator
{
public:
  range_iterator() = default;
  range_iterator(Range* range, std::size_t idx)
    : range_{range}
    , idx_{idx}
  {}

  template <typename Self>
  auto operator*(this Self&& self) -> decltype(auto)
  {
    auto const idx = self.idx_;
    return (*std::forward<Self>(self).range_)[idx];
  }

  template <typename Self>
  auto operator->(this Self&& self)
  {
    decltype(auto) ref = *std::forward<Self>(self);
    if constexpr (std::is_reference_v<decltype(ref)>) {
      return std::addressof(ref);
    }
    else {
      return arrow_proxy{std::move(ref)};
    }
  }

  template <typename Self>
  auto operator[](this Self&& self, std::size_t n) -> decltype(auto)
  {
    return *(std::forward<Self>(self) + n);
  }

  auto operator++() -> range_iterator&
  {
    idx_ += 1;
    return *this;
  }

  auto operator++(int) -> range_iterator
  {
    auto copy = range_iterator{*this};
    ++*this;
    return copy;
  }

  auto operator--() -> range_iterator&
  {
    idx_ -= 1;
    return *this;
  }

  auto operator--(int) -> range_iterator
  {
    auto copy = range_iterator{*this};
    --*this;
    return copy;
  }

  auto operator+=(std::ptrdiff_t n) -> range_iterator&
  {
    idx_ += n;
    return *this;
  }

  auto operator-=(std::ptrdiff_t n) -> range_iterator&
  {
    idx_ -= n;
    return *this;
  }

private:
  friend auto operator-(range_iterator const& left, range_iterator const& right)
    -> std::ptrdiff_t
  {
    assert(left.range_ == right.range_);
    return left.idx_ - right.idx_;
  }

  friend auto operator+(range_iterator i, std::ptrdiff_t n) -> range_iterator
  {
    return i += n;
  }
  friend auto operator-(range_iterator i, std::ptrdiff_t n) -> range_iterator
  {
    return i -= n;
  }
  friend auto operator+(std::ptrdiff_t n, range_iterator i) -> range_iterator
  {
    return i += n;
  }

  friend auto operator<=>(
    range_iterator const& left, range_iterator const& right
  )
  {
    assert(left.range_ == right.range_);
    return left.idx_ <=> right.idx_;
  }

  friend auto operator==(
    range_iterator const& left, range_iterator const& right
  ) -> bool
  {
    assert(left.range_ == right.range_);
    return left.idx_ == right.idx_;
  }

  Range* range_ = nullptr;
  std::size_t idx_ = 0;
};

template <class Derived>
class range
{
public:
  using iterator = range_iterator<Derived>;
  using const_iterator = range_iterator<Derived const>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using reverse_const_iterator = std::reverse_iterator<const_iterator>;

  template <typename Self>
  [[nodiscard]] auto empty(this Self const& self) -> bool
  {
    return self.size() == 0;
  }

  template <typename Self>
  [[nodiscard]] auto begin(this Self& self)
  {
    return range_iterator<Self>{&self, 0};
  }

  template <typename Self>
  [[nodiscard]] auto end(this Self& self)
  {
    return range_iterator<Self>{&self, self.size()};
  }

  template <typename Self>
  [[nodiscard]] auto cbegin(this Self const& self)
  {
    return self.begin();
  }

  template <typename Self>
  [[nodiscard]] auto cend(this Self const& self)
  {
    return self.end();
  }

  template <typename Self>
  [[nodiscard]] auto rbegin(this Self& self)
  {
    return std::reverse_iterator{self.end()};
  }

  template <typename Self>
  [[nodiscard]] auto rend(this Self& self)
  {
    return std::reverse_iterator{self.begin()};
  }

  template <typename Self>
  [[nodiscard]] auto crbegin(this Self const& self)
  {
    return self.rbegin();
  }

  template <typename Self>
  [[nodiscard]] auto crend(this Self const& self)
  {
    return self.rend();
  }

  template <typename Self>
  [[nodiscard]] auto front(this Self& self) -> decltype(auto)
  {
    return self[std::size_t(0)];
  }

  template <typename Self>
  [[nodiscard]] auto back(this Self& self) -> decltype(auto)
  {
    return self[self.size() - 1];
  }

private:
  range() = default;
  friend Derived;
};

struct owned_tag
{};

constexpr auto const owned = owned_tag{};

struct get_impl_;

template <typename T, T* (*Retain)(T*), void (*Release)(T*)>
class arc
{
protected:
  using base = arc;

  arc() noexcept
    : ptr_{nullptr}
  {}

public:
  arc(T* ptr, owned_tag /*tag*/) noexcept
    : ptr_{ptr}
  {}

protected:
  arc(arc const& other) noexcept
    : ptr_{Retain(other.ptr_)}
  {}

  arc(arc&& other) noexcept
    : ptr_{std::exchange(other.ptr_, nullptr)}
  {}

  auto operator=(arc const& other) noexcept -> arc&
  {
    if (this != &other) {
      reset(Retain(other.ptr_));
    }
    return *this;
  }

  auto operator=(arc&& other) noexcept -> arc&
  {
    if (this != &other) {
      reset(other.ptr_);
      other.ptr_ = nullptr;
    }
    return *this;
  }

  ~arc() { Release(ptr_); }

  [[nodiscard]] auto impl() -> T* { return ptr_; }
  [[nodiscard]] auto impl() const -> T const* { return ptr_; }

private:
  void reset(T* ptr = nullptr) noexcept
  {
    if (ptr_ != ptr) {
      Release(ptr_);
      ptr_ = ptr;
    }
  }

  T* ptr_;
  friend struct get_impl_;
};

struct get_impl_
{
  template <class T, T* (*Retain)(T*), void (*Release)(T*)>
  auto operator()(arc<T, Retain, Release>& arg) const -> T*
  {
    return arg.ptr_;
  }

  template <class T, T* (*Retain)(T const*), void (*Release)(T*)>
  auto operator()(arc<T, Retain, Release> const& arg) const -> T const*
  {
    return arg.ptr_;
  }
};

constexpr auto const get_impl = get_impl_{};

inline auto as_bytes(void const* data, std::size_t len)
{
  return std::span{reinterpret_cast<std::byte const*>(data), len};
}

template <typename Smart>
class out_ptr
{
public:
  using element_type = typename Smart::element_type;

  explicit out_ptr(Smart& smart)
    : smart_(smart)
  {}

  ~out_ptr() { smart_.reset(raw_); }

  operator element_type**() { return &raw_; }

private:
  Smart& smart_;
  element_type* raw_ = nullptr;
};

struct error_deleter
{
  void operator()(BtcK_Error* error) const { BtcK_Error_Free(error); }
};

struct error : std::unique_ptr<BtcK_Error, error_deleter>
{
  [[nodiscard]] auto code() const { return BtcK_Error_Code(get()); }
  [[nodiscard]] auto domain() const { return BtcK_Error_Domain(get()); }
  [[nodiscard]] auto message() const { return BtcK_Error_Message(get()); }
};

[[noreturn]] inline void translate_error(error const& err)
{
  using namespace std::literals;
  if (err.domain() == "Memory"sv) {
    throw std::bad_alloc();
  }
  throw std::runtime_error{err.message()};
}

template <typename Function, typename... Args>
auto invoke(Function function, Args... args)
{
  auto err = error{};
  auto const result = function(args..., out_ptr{err});
  if (err != nullptr) [[unlikely]] {
    translate_error(err);
  }
  return result;
}

}  // namespace btck::detail

template <typename Range>
struct std::iterator_traits<btck::detail::range_iterator<Range>>
{
  using value_type = typename Range::value_type;
  using reference =
    decltype(*std::declval<btck::detail::range_iterator<Range>>());
  using pointer =
    decltype(std::declval<btck::detail::range_iterator<Range>>().operator->());
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::random_access_iterator_tag;
};

namespace btck {

class ScriptPubkey
  : public detail::arc<
      BtcK_ScriptPubkey,
      BtcK_ScriptPubkey_Retain,
      BtcK_ScriptPubkey_Release>
{
public:
  using base::base;

  ScriptPubkey(std::span<std::byte const> raw)
    : base{
        detail::invoke(BtcK_ScriptPubkey_New, raw.data(), raw.size()),
        detail::owned
      }
  {}

private:
  friend auto operator==(ScriptPubkey const& left, ScriptPubkey const& right)
    -> bool
  {
    return BtcK_ScriptPubkey_Equal(left.impl(), right.impl());
  }

  friend auto as_bytes(ScriptPubkey const& self) -> std::span<std::byte const>
  {
    auto len = std::size_t{};
    auto const* data = BtcK_ScriptPubkey_AsBytes(self.impl(), &len);
    return detail::as_bytes(data, len);
  }
};

class TransactionOutput
  : public detail::arc<
      BtcK_TransactionOutput,
      BtcK_TransactionOutput_Retain,
      BtcK_TransactionOutput_Release>
{
public:
  using base::base;

  TransactionOutput(std::int64_t amount, ScriptPubkey script_pubkey)
    : base{
        detail::invoke(
          BtcK_TransactionOutput_New, amount, detail::get_impl(script_pubkey)
        ),
        detail::owned
      }
  {}

  [[nodiscard]] auto amount() const -> std::int64_t
  {
    return BtcK_TransactionOutput_GetAmount(this->impl());
  }

  [[nodiscard]] auto script_pubkey() const -> ScriptPubkey
  {
    return {
      detail::invoke(BtcK_TransactionOutput_GetScriptPubkey, this->impl()),
      detail::owned
    };
  }
};

class Transaction
  : public detail::
      arc<BtcK_Transaction, BtcK_Transaction_Retain, BtcK_Transaction_Release>
  , public detail::range<Transaction const>
{
public:
  using value_type = TransactionOutput;

  using base::base;

  Transaction(std::span<std::byte const> raw)
    : base{
        detail::invoke(BtcK_Transaction_New, raw.data(), raw.size()),
        detail::owned
      }
  {}

  [[nodiscard]] auto size() const -> std::size_t
  {
    return BtcK_Transaction_GetSize(this->impl());
  }
  [[nodiscard]] auto operator[](std::size_t idx) const -> value_type
  {
    return {BtcK_Transaction_At(this->impl(), idx), detail::owned};
  }

private:
  friend auto as_bytes(Transaction const& self) -> std::span<std::byte const>
  {
    auto len = std::size_t{};
    auto const* data = BtcK_Transaction_AsBytes(self.impl(), &len);
    return detail::as_bytes(data, len);
  }

  friend auto to_string(Transaction const& self) -> std::string_view
  {
    auto len = std::size_t{};
    char const* data = BtcK_Transaction_ToString(self.impl(), &len);
    return {data, len};
  }
};

enum class VerificationError : std::uint8_t
{
  TX_INPUT_INDEX,
  INVALID_FLAGS,
  INVALID_FLAGS_COMBINATION,
  SPENT_OUTPUTS_REQUIRED,
  SPENT_OUTPUTS_MISMATCH,
};

auto verify(
  ScriptPubkey const& script_pubkey,
  std::int64_t amount,
  Transaction const& tx_to,
  std::span<TransactionOutput const> spent_outputs,
  unsigned int input_index,
  unsigned int flags
) -> std::optional<VerificationError>;

class BlockHash
{
public:
  static constexpr auto size = std::size_t{BtcK_BlockHash_SIZE};

  BlockHash() = default;
  BlockHash(std::span<std::byte const, size> raw)
  {
    BtcK_BlockHash_Init(&this->impl_, raw.data(), raw.size());
  }

private:
  friend auto operator==(BlockHash const& lhs, BlockHash const& rhs) -> bool
  {
    return std::ranges::equal(lhs.impl_.data, rhs.impl_.data);
  }

  friend auto as_bytes(BlockHash const& self)
    -> std::span<std::byte const, size>
  {
    return as_bytes(std::span{self.impl_.data});
  }

  BtcK_BlockHash impl_;
  friend class Block;
  friend class Chain;
};

class Block
  : public detail::arc<BtcK_Block, BtcK_Block_Retain, BtcK_Block_Release>
  , public detail::range<Block const>
{
public:
  using value_type = Transaction;

  using base::base;

  Block(std::span<std::byte const> raw)
    : base{
        detail::invoke(BtcK_Block_New, raw.data(), raw.size()), detail::owned
      }
  {}

  [[nodiscard]] auto hash() const -> BlockHash
  {
    auto hash = BlockHash{};
    BtcK_Block_GetHash(this->impl(), &hash.impl_);
    return hash;
  }

  [[nodiscard]] auto size() const -> std::size_t
  {
    return BtcK_Block_GetSize(this->impl());
  }
  [[nodiscard]] auto operator[](std::size_t idx) const -> value_type
  {
    return {BtcK_Block_At(this->impl(), idx), detail::owned};
  }

private:
  friend auto as_bytes(Block const& self) -> std::span<std::byte const>
  {
    auto len = std::size_t{};
    auto const* data = BtcK_Block_AsBytes(self.impl(), &len);
    return detail::as_bytes(data, len);
  }
};

enum class ValidationState : std::uint8_t
{
  VALID,
  INVALID,
  ERROR,
};

enum class ValidationResult : std::uint8_t
{
  RESULT_UNSET,    //!< initial value. Block has not yet been rejected
  CONSENSUS,       //!< invalid by consensus rules (excluding any below reasons)
  CACHED_INVALID,  //!< this block was cached as being invalid and we didn't
                   //!< store the reason why
  INVALID_HEADER,  //!< invalid proof of work or time too old
  MUTATED,  //!< the block's data didn't match the data committed to by the PoW
  MISSING_PREV,  //!< We don't have the previous block the checked one is built
                 //!< on
  INVALID_PREV,  //!< A block this one builds on is invalid
  TIME_FUTURE,  //!< block timestamp was > 2 hours in the future (or our clock is
                //!< bad)
  HEADER_LOW_WORK,  //!< the block header may be on a too-little-work chain
};

using Validation =
  std::function<void(Block const&, ValidationState, ValidationResult)>;

enum class chain_type : BtcK_ChainType
{
  mainnet = BtcK_ChainType_MAINNET,
  testnet = BtcK_ChainType_TESTNET,
  testnet_4 = BtcK_ChainType_TESTNET_4,
  signet = BtcK_ChainType_SIGNET,
  regtest = BtcK_ChainType_REGTEST,
};

using Log = std::function<void(std::string_view)>;

enum class LogFlags : std::uint8_t
{
  TIMESTAMPS,
  TIME_MICROS,
  THREADNAMES,
  SOURCE_LOCATIONS,
  CATEGORY_LEVELS,
};

class Logger
{
public:
  Logger(Log log, LogFlags flags);
};

template <typename T>
class KernelNotifications
{
public:
  KernelNotifications();
  // virtual void BlockTipHandler(kernel_SynchronizationState state, kernel_BlockIndex const* index) {}
  // virtual void HeaderTipHandler(kernel_SynchronizationState state, int64_t height, int64_t timestamp, bool presync) {}
  // virtual void ProgressHandler(std::string_view title, int progress_percent, bool resume_possible) {}
  // virtual void WarningSetHandler(kernel_Warning warning, std::string_view message) {}
  // virtual void WarningUnsetHandler(kernel_Warning warning) {}
  // virtual void FlushErrorHandler(std::string_view error) {}
  // virtual void FatalErrorHandler(std::string_view error) {}
};

class Chain : public detail::range<Chain const>
{
public:
  // poor man's cpp support for named arguments
  struct KwArgs
  {
    KwArgs();
    auto chain_type(chain_type arg) && -> KwArgs;
    auto validation(Validation arg) && -> KwArgs;

    template <typename T>
    auto notifications(KernelNotifications<T> arg) && -> KwArgs;

    auto SetWorkerThreads(int worker_threads) && -> KwArgs;
    auto SetWipeDbs(bool wipe_block_tree, bool wipe_chainstate) && -> KwArgs;
    auto SetBlockTreeDbInMemory(bool block_tree_db_in_memory) && -> KwArgs;
    auto SetChainstateDbInMemory(bool chainstate_db_in_memory) && -> KwArgs;
  };

  Chain(
    std::string_view data_dir, std::string_view blocks_dir, KwArgs kwargs = {}
  );

  // auto ImportBlocks(std::span<std::string const> paths) -> bool;
  // auto ProcessBlock(Block const& block, bool* new_block) -> bool;

  using value_type = Block;
  [[nodiscard]] auto size() const -> std::size_t
  {
    return BtcK_Chain_GetSize(this->impl_.get());
  }
  [[nodiscard]] auto operator[](std::size_t height) const -> value_type
  {
    return {BtcK_Chain_At(this->impl_.get(), height), detail::owned};
  }

  [[nodiscard]] auto find(BlockHash const& block_hash) const -> iterator
  {
    std::ptrdiff_t const idx =
      BtcK_Chain_Find(this->impl_.get(), &block_hash.impl_);
    return (idx == -1) ? this->end() : this->begin() + idx;
  }

private:
  struct Deleter
  {
    void operator()(BtcK_Chain* chain) const { BtcK_Chain_Release(chain); }
  };

  std::unique_ptr<BtcK_Chain, Deleter> impl_;
};

}  // namespace btck
