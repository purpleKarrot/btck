// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iterator>
#include <memory>
#include <optional>
#include <span>
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

  auto operator*() -> decltype(auto) { return (*range_)[idx_]; }
  auto operator*() const -> decltype(auto) { return (*range_)[idx_]; }

  auto operator->()
  {
    decltype(auto) ref = **this;
    if constexpr (std::is_reference_v<decltype(ref)>) {
      return std::addressof(ref);
    }
    else {
      return arrow_proxy{std::move(ref)};
    }
  }

  auto operator->() const
  {
    decltype(auto) ref = **this;
    if constexpr (std::is_reference_v<decltype(ref)>) {
      return std::addressof(ref);
    }
    else {
      return arrow_proxy{std::move(ref)};
    }
  }

  auto operator[](std::size_t n) -> decltype(auto) { return *(*this + n); }
  auto operator[](std::size_t n) const -> decltype(auto) { return *(*this + n); }

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
  friend auto operator-(range_iterator const& left, range_iterator const& right) -> std::ptrdiff_t
  {
    assert(left.range_ == right.range_);
    return left.idx_ - right.idx_;
  }

  friend auto operator+(range_iterator i, std::ptrdiff_t n) -> range_iterator { return i += n; }
  friend auto operator-(range_iterator i, std::ptrdiff_t n) -> range_iterator { return i -= n; }
  friend auto operator+(std::ptrdiff_t n, range_iterator i) -> range_iterator { return i += n; }

  friend auto operator<=>(range_iterator const& left, range_iterator const& right)
  {
    assert(left.range_ == right.range_);
    return left.idx_ <=> right.idx_;
  }

  friend auto operator==(range_iterator const& left, range_iterator const& right) -> bool
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

  [[nodiscard]] auto empty() const -> bool { return self().size() == 0; }

  [[nodiscard]] auto begin() -> iterator { return {&self(), 0}; }
  [[nodiscard]] auto end() -> iterator { return {&self(), self().size()}; }

  [[nodiscard]] auto begin() const -> const_iterator { return {&self(), 0}; }
  [[nodiscard]] auto end() const -> const_iterator { return {&self(), self().size()}; }
  [[nodiscard]] auto cbegin() const -> const_iterator { return {&self(), 0}; }
  [[nodiscard]] auto cend() const -> const_iterator { return {&self(), self().size()}; }

  [[nodiscard]] auto rbegin() -> reverse_iterator { return end(); }
  [[nodiscard]] auto rend() -> reverse_iterator { return begin(); }

  [[nodiscard]] auto rbegin() const -> reverse_const_iterator { return end(); }
  [[nodiscard]] auto rend() const -> reverse_const_iterator { return begin(); }
  [[nodiscard]] auto crbegin() const -> reverse_const_iterator { return end(); }
  [[nodiscard]] auto crend() const -> reverse_const_iterator { return begin(); }

  [[nodiscard]] auto front() -> decltype(auto) { return self()[std::size_t(0)]; }
  [[nodiscard]] auto front() const -> decltype(auto) { return self()[std::size_t(0)]; }
  [[nodiscard]] auto back() -> decltype(auto) { return self()[self().size() - 1]; }
  [[nodiscard]] auto back() const -> decltype(auto) { return self()[self().size() - 1]; }

private:
  range() = default;
  friend Derived;

  [[nodiscard]] auto self() -> Derived& { return static_cast<Derived&>(*this); }
  [[nodiscard]] auto self() const -> Derived const& { return static_cast<Derived const&>(*this); }
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

}  // namespace btck::detail

template <typename Range>
struct std::iterator_traits<btck::detail::range_iterator<Range>>
{
  using value_type = typename Range::value_type;
  using reference = decltype(*std::declval<btck::detail::range_iterator<Range>>());
  using pointer = decltype(std::declval<btck::detail::range_iterator<Range>>().operator->());
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::random_access_iterator_tag;
};

namespace btck {

class ScriptPubkey : public detail::arc<btck_ScriptPubkey, btck_ScriptPubkey_Retain, btck_ScriptPubkey_Release>
{
public:
  using base::base;

  ScriptPubkey(std::span<std::byte const> raw)
    : base{btck_ScriptPubkey_New(raw.data(), raw.size()), detail::owned}
  {}

private:
  friend auto operator==(ScriptPubkey const& left, ScriptPubkey const& right) -> bool
  {
    return btck_ScriptPubkey_Equal(left.impl(), right.impl());
  }

  friend auto as_bytes(ScriptPubkey const& self) -> std::span<std::byte const>
  {
    auto len = std::size_t{};
    auto const* data = btck_ScriptPubkey_AsBytes(self.impl(), &len);
    return {reinterpret_cast<std::byte const*>(data), len};
  }
};

class TransactionOutput
  : public detail::arc<btck_TransactionOutput, btck_TransactionOutput_Retain, btck_TransactionOutput_Release>
{
public:
  using base::base;

  TransactionOutput(std::int64_t amount, ScriptPubkey script_pubkey)
    : base{btck_TransactionOutput_New(amount, detail::get_impl(script_pubkey)), detail::owned}
  {}

  [[nodiscard]] auto amount() const -> std::int64_t { return btck_TransactionOutput_GetAmount(this->impl()); }

  [[nodiscard]] auto script_pubkey() const -> ScriptPubkey
  {
    return {btck_TransactionOutput_GetScriptPubkey(this->impl()), detail::owned};
  }
};

class Transaction
  : public detail::arc<btck_Transaction, btck_Transaction_Retain, btck_Transaction_Release>
  , public detail::range<Transaction const>
{
public:
  using value_type = TransactionOutput;

  using base::base;

  Transaction(std::span<std::byte const> raw)
    : base{btck_Transaction_New(raw.data(), raw.size()), detail::owned}
  {}

  [[nodiscard]] auto size() const -> std::size_t { return btck_Transaction_GetSize(this->impl()); }
  [[nodiscard]] auto operator[](std::size_t idx) const -> value_type
  {
    return {btck_Transaction_At(this->impl(), idx), detail::owned};
  }

private:
  friend auto as_bytes(Transaction const& self) -> std::span<std::byte const>
  {
    auto len = std::size_t{};
    auto const* data = btck_Transaction_AsBytes(self.impl(), &len);
    return {reinterpret_cast<std::byte const*>(data), len};
  }

  friend auto to_string(Transaction const& self) -> std::string_view
  {
    auto len = std::size_t{};
    char const* data = btck_Transaction_ToString(self.impl(), &len);
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
  static constexpr auto size = std::size_t{btck_BlockHash_SIZE};

  BlockHash() = default;
  BlockHash(std::span<std::byte const, size> raw) { btck_BlockHash_Init(&this->impl_, raw.data(), raw.size()); }

private:
  friend auto operator==(BlockHash const& lhs, BlockHash const& rhs) -> bool
  {
    return std::ranges::equal(lhs.impl_.data, rhs.impl_.data);
  }

  friend auto as_bytes(BlockHash const& self) -> std::span<std::byte const, size>
  {
    return as_bytes(std::span{self.impl_.data});
  }

  btck_BlockHash impl_;
  friend class Block;
  friend class Chain;
};

class Block
  : public detail::arc<btck_Block, btck_Block_Retain, btck_Block_Release>
  , public detail::range<Block const>
{
public:
  using value_type = Transaction;

  using base::base;

  Block(std::span<std::byte const> raw)
    : base{btck_Block_New(raw.data(), raw.size()), detail::owned}
  {}

  [[nodiscard]] auto hash() const -> BlockHash
  {
    auto hash = BlockHash{};
    btck_Block_GetHash(this->impl(), &hash.impl_);
    return hash;
  }

  [[nodiscard]] auto size() const -> std::size_t { return btck_Block_GetSize(this->impl()); }
  [[nodiscard]] auto operator[](std::size_t idx) const -> value_type
  {
    return {btck_Block_At(this->impl(), idx), detail::owned};
  }

private:
  friend auto as_bytes(Block const& self) -> std::span<std::byte const>
  {
    auto len = std::size_t{};
    auto const* data = btck_Block_AsBytes(self.impl(), &len);
    return {reinterpret_cast<std::byte const*>(data), len};
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
  RESULT_UNSET,     //!< initial value. Block has not yet been rejected
  CONSENSUS,        //!< invalid by consensus rules (excluding any below reasons)
  CACHED_INVALID,   //!< this block was cached as being invalid and we didn't
                    //!< store the reason why
  INVALID_HEADER,   //!< invalid proof of work or time too old
  MUTATED,          //!< the block's data didn't match the data committed to by the PoW
  MISSING_PREV,     //!< We don't have the previous block the checked one is built
                    //!< on
  INVALID_PREV,     //!< A block this one builds on is invalid
  TIME_FUTURE,      //!< block timestamp was > 2 hours in the future (or our clock is
                    //!< bad)
  HEADER_LOW_WORK,  //!< the block header may be on a too-little-work chain
};

using Validation = std::function<void(Block const&, ValidationState, ValidationResult)>;

enum class ChainType : std::uint8_t
{
  MAINNET = 0,
  TESTNET,
  TESTNET_4,
  SIGNET,
  REGTEST,
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
    auto chain_type(ChainType arg) && -> KwArgs;
    auto validation(Validation arg) && -> KwArgs;

    template <typename T>
    auto notifications(KernelNotifications<T> arg) && -> KwArgs;

    auto SetWorkerThreads(int worker_threads) && -> KwArgs;
    auto SetWipeDbs(bool wipe_block_tree, bool wipe_chainstate) && -> KwArgs;
    auto SetBlockTreeDbInMemory(bool block_tree_db_in_memory) && -> KwArgs;
    auto SetChainstateDbInMemory(bool chainstate_db_in_memory) && -> KwArgs;
  };

  Chain(std::string_view data_dir, std::string_view blocks_dir, KwArgs kwargs = {});

  // auto ImportBlocks(std::span<std::string const> paths) -> bool;
  // auto ProcessBlock(Block const& block, bool* new_block) -> bool;

  using value_type = Block;
  [[nodiscard]] auto size() const -> std::size_t { return btck_Chain_GetSize(this->impl_.get()); }
  [[nodiscard]] auto operator[](std::size_t height) const -> value_type
  {
    return {btck_Chain_At(this->impl_.get(), height), detail::owned};
  }

  [[nodiscard]] auto find(BlockHash const& block_hash) const -> iterator
  {
    std::ptrdiff_t const idx = btck_Chain_Find(this->impl_.get(), &block_hash.impl_);
    return (idx == -1) ? this->end() : this->begin() + idx;
  }

private:
  struct Deleter
  {
    void operator()(btck_Chain* chain) const { btck_Chain_Release(chain); }
  };

  std::unique_ptr<btck_Chain, Deleter> impl_;
};

}  // namespace btck
