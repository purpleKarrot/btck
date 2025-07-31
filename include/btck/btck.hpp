// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <btck/btck.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iterator>
#include <memory>
#include <new>
#include <span>
#include <stdexcept>
#include <string_view>
#include <system_error>
#include <utility>

/******************************************************************************/
// MARK: Range Mixin

namespace btck::detail {

template <typename T> struct arrow_proxy {
  auto operator->() -> T* { return &r; }
  T r;
};

template <typename T> arrow_proxy(T) -> arrow_proxy<T>;

template <typename Range> class range_iterator
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
  auto operator[](std::size_t n) const -> decltype(auto)
  {
    return *(*this + n);
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
    range_iterator const& left, range_iterator const& right)
  {
    assert(left.range_ == right.range_);
    return left.idx_ <=> right.idx_;
  }

  friend auto operator==(
    range_iterator const& left, range_iterator const& right) -> bool
  {
    assert(left.range_ == right.range_);
    return left.idx_ == right.idx_;
  }

  Range* range_ = nullptr;
  std::size_t idx_ = 0;
};

template <class Derived> class range
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
  [[nodiscard]] auto end() const -> const_iterator
  {
    return {&self(), self().size()};
  }
  [[nodiscard]] auto cbegin() const -> const_iterator { return {&self(), 0}; }
  [[nodiscard]] auto cend() const -> const_iterator
  {
    return {&self(), self().size()};
  }

  [[nodiscard]] auto rbegin() -> reverse_iterator { return end(); }
  [[nodiscard]] auto rend() -> reverse_iterator { return begin(); }

  [[nodiscard]] auto rbegin() const -> reverse_const_iterator { return end(); }
  [[nodiscard]] auto rend() const -> reverse_const_iterator { return begin(); }
  [[nodiscard]] auto crbegin() const -> reverse_const_iterator { return end(); }
  [[nodiscard]] auto crend() const -> reverse_const_iterator { return begin(); }

  [[nodiscard]] auto front() -> decltype(auto)
  {
    return self()[std::size_t(0)];
  }
  [[nodiscard]] auto front() const -> decltype(auto)
  {
    return self()[std::size_t(0)];
  }
  [[nodiscard]] auto back() -> decltype(auto)
  {
    return self()[self().size() - 1];
  }
  [[nodiscard]] auto back() const -> decltype(auto)
  {
    return self()[self().size() - 1];
  }

private:
  range() = default;
  friend Derived;

  [[nodiscard]] auto self() -> Derived& { return static_cast<Derived&>(*this); }
  [[nodiscard]] auto self() const -> Derived const&
  {
    return static_cast<Derived const&>(*this);
  }
};

}  // namespace btck::detail

template <typename Range>
struct std::iterator_traits<btck::detail::range_iterator<Range>> {
  using value_type = typename Range::value_type;
  using reference =
    decltype(*std::declval<btck::detail::range_iterator<Range>>());
  using pointer =
    decltype(std::declval<btck::detail::range_iterator<Range>>().operator->());
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::random_access_iterator_tag;
};

/******************************************************************************/
// MARK: ARC Mixin

namespace btck::detail {

struct owned_tag {};

constexpr auto const owned = owned_tag{};

struct get_impl_;

template <typename T, T* (*Retain)(T*), void (*Release)(T*)> class arc
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

struct get_impl_ {
  template <class T, T* (*Retain)(T*), void (*Release)(T*)>
  auto operator()(arc<T, Retain, Release>& arg) const -> T*
  {
    return arg.ptr_;
  }

  template <class T, T* (*Retain)(T*), void (*Release)(T*)>
  auto operator()(arc<T, Retain, Release> const& arg) const -> T const*
  {
    return arg.ptr_;
  }

  template <class T, T* (*Retain)(T*), void (*Release)(T*)>
  auto operator()(arc<T, Retain, Release> const* arg) const -> T const* const*
  {
    return &arg->ptr_;
  }
};

constexpr auto const get_impl = get_impl_{};

}  // namespace btck::detail

/******************************************************************************/
// MARK: out_ptr

namespace btck::detail {

template <typename Smart> class out_ptr
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

}  // namespace btck::detail

/******************************************************************************/
// MARK: Flag Operators

namespace btck {
namespace detail {

template <typename E> struct is_flag_enum : std::false_type {};

template <typename E>
concept flag_enum = is_flag_enum<E>::value;

}  // namespace detail

template <detail::flag_enum E> constexpr auto operator|(E left, E right)
{
  using U = std::underlying_type_t<E>;
  return static_cast<E>(static_cast<U>(left) | static_cast<U>(right));
}

template <detail::flag_enum E> constexpr auto operator&(E left, E right)
{
  using U = std::underlying_type_t<E>;
  return static_cast<E>(static_cast<U>(left) & static_cast<U>(right));
}

template <detail::flag_enum E> constexpr auto operator^(E left, E right)
{
  using U = std::underlying_type_t<E>;
  return static_cast<E>(static_cast<U>(left) ^ static_cast<U>(right));
}

template <detail::flag_enum E> constexpr auto operator~(E e)
{
  using U = std::underlying_type_t<E>;
  return static_cast<E>(~static_cast<U>(e));
}

template <detail::flag_enum E>
constexpr auto operator|=(E& left, E right) -> decltype(auto)
{
  left = left | right;
  return left;
}

template <detail::flag_enum E>
constexpr auto operator&=(E& left, E right) -> decltype(auto)
{
  left = left & right;
  return left;
}

template <detail::flag_enum E>
constexpr auto operator^=(E& left, E right) -> decltype(auto)
{
  left = left ^ right;
  return left;
}

}  // namespace btck

/******************************************************************************/
// MARK: VerificationError

namespace btck {

enum class verification_error : BtcK_VerificationError {
  tx_input_index = BtcK_VerificationError_TX_INPUT_INDEX,
  invalid_flags = BtcK_VerificationError_INVALID_FLAGS,
  invalid_flags_combination = BtcK_VerificationError_INVALID_FLAGS_COMBINATION,
  spent_outputs_required = BtcK_VerificationError_SPENT_OUTPUTS_REQUIRED,
  spent_outputs_mismatch = BtcK_VerificationError_SPENT_OUTPUTS_MISMATCH,
};

namespace detail {

inline struct : std::error_category {
  [[nodiscard]] auto name() const noexcept -> char const* override
  {
    return "VerificationError";
  }

  [[nodiscard]] auto message(int ev) const -> std::string override
  {
    return BtcK_VerificationError_Message(ev);
  }
} const verification_error_category;

}  // namespace detail

inline auto make_error_code(verification_error err) -> std::error_code
{
  return {static_cast<int>(err), detail::verification_error_category};
}

}  // namespace btck

template <>
struct std::is_error_code_enum<btck::verification_error> : std::true_type {};

/******************************************************************************/
// MARK: VerificationFlags

namespace btck {

enum class verification_flags : BtcK_VerificationFlags {
  none = BtcK_VerificationFlags_NONE,
  p2sh = BtcK_VerificationFlags_P2SH,
  dersig = BtcK_VerificationFlags_DERSIG,
  nulldummy = BtcK_VerificationFlags_NULLDUMMY,
  checklocktimeverify = BtcK_VerificationFlags_CHECKLOCKTIMEVERIFY,
  checksequenceverify = BtcK_VerificationFlags_CHECKSEQUENCEVERIFY,
  witness = BtcK_VerificationFlags_WITNESS,
  taproot = BtcK_VerificationFlags_TAPROOT,
  all = BtcK_VerificationFlags_ALL,
};

inline auto to_string(verification_flags flags)
{
  auto const cflags = static_cast<BtcK_VerificationFlags>(flags);
  auto const len = BtcK_VerificationFlags_ToString(cflags, nullptr, 0);
  if (len < 0) {
    throw std::runtime_error("BtcK_VerificationFlags_ToString failed");
  }
  auto buf = std::string(static_cast<std::string::size_type>(len), '\0');
  BtcK_VerificationFlags_ToString(cflags, buf.data(), len + 1);
  return buf;
}

}  // namespace btck

template <>
struct btck::detail::is_flag_enum<btck::verification_flags> : std::true_type {};

/******************************************************************************/

namespace btck::detail {

inline auto as_bytes(void const* data, std::size_t len)
{
  return std::span{reinterpret_cast<std::byte const*>(data), len};
}

struct error_deleter {
  void operator()(BtcK_Error* error) const { BtcK_Error_Free(error); }
};

struct error : std::unique_ptr<BtcK_Error, error_deleter> {
  [[nodiscard]] auto code() const { return BtcK_Error_Code(get()); }
  [[nodiscard]] auto domain() const { return BtcK_Error_Domain(get()); }
  [[nodiscard]] auto message() const { return BtcK_Error_Message(get()); }
};

inline void throw_domain(error const& err, std::error_category const& domain)
{
  if (std::strcmp(err.domain(), domain.name()) == 0) {
    throw std::system_error(err.code(), domain);
  }
}

[[noreturn]] inline void translate_error(error const& err)
{
  using namespace std::literals;
  if (err.domain() == "Memory"sv) {
    throw std::bad_alloc();
  }
  throw_domain(err, std::generic_category());  // TODO: Is this portable?
  throw_domain(err, std::system_category());   // TODO: Is this portable?
  throw_domain(err, detail::verification_error_category);
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

/******************************************************************************/
// MARK: ScriptPubkey

namespace btck {

class ScriptPubkey
  : public detail::arc<
      BtcK_ScriptPubkey, BtcK_ScriptPubkey_Retain, BtcK_ScriptPubkey_Release>
{
public:
  using base::base;

  ScriptPubkey(std::span<std::byte const> raw)
    : base{
        detail::invoke(BtcK_ScriptPubkey_New, raw.data(), raw.size()),
        detail::owned}
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

}  // namespace btck

/******************************************************************************/
// MARK: TransactionOutput

namespace btck {

class TransactionOutput
  : public detail::arc<
      BtcK_TransactionOutput, BtcK_TransactionOutput_Retain,
      BtcK_TransactionOutput_Release>
{
public:
  using base::base;

  TransactionOutput(std::int64_t amount, ScriptPubkey const& script_pubkey)
    : base{
        detail::invoke(
          BtcK_TransactionOutput_New, amount, detail::get_impl(script_pubkey)),
        detail::owned}
  {}

  [[nodiscard]] auto amount() const -> std::int64_t
  {
    return BtcK_TransactionOutput_GetAmount(this->impl());
  }

  [[nodiscard]] auto script_pubkey() const -> ScriptPubkey
  {
    return {
      detail::invoke(BtcK_TransactionOutput_GetScriptPubkey, this->impl()),
      detail::owned};
  }
};

}  // namespace btck

/******************************************************************************/
// MARK: Transaction

namespace btck {

class Transaction
  : public detail::arc<
      BtcK_Transaction, BtcK_Transaction_Retain, BtcK_Transaction_Release>
  , public detail::range<Transaction const>
{
public:
  using value_type = TransactionOutput;

  using base::base;

  Transaction(std::span<std::byte const> raw)
    : base{
        detail::invoke(BtcK_Transaction_New, raw.data(), raw.size()),
        detail::owned}
  {}

  [[nodiscard]] auto size() const -> std::size_t
  {
    return BtcK_Transaction_NumOutputs(this->impl());
  }
  [[nodiscard]] auto operator[](std::size_t idx) const -> value_type
  {
    return {BtcK_Transaction_GetOutput(this->impl(), idx), detail::owned};
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

}  // namespace btck

/******************************************************************************/
// MARK: Verify

namespace btck {
namespace detail {

struct verify_fn {
  auto operator()(
    ScriptPubkey const& script_pubkey, std::int64_t amount,
    Transaction const& tx_to, std::span<TransactionOutput const> spent_outputs,
    unsigned int input_index, verification_flags flags) const -> bool
  {
    return detail::invoke(
      BtcK_Verify, detail::get_impl(script_pubkey), amount,
      detail::get_impl(tx_to),
      (spent_outputs.empty() ? nullptr
                             : detail::get_impl(spent_outputs.data())),
      spent_outputs.size(), input_index,
      static_cast<BtcK_VerificationFlags>(flags));
  }
};

}  // namespace detail

constexpr auto const verify = detail::verify_fn{};

}  // namespace btck

/******************************************************************************/
// MARK: BlockHash

namespace btck {

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
  friend auto operator==(BlockHash const& left, BlockHash const& right) -> bool
  {
    return std::ranges::equal(left.impl_.data, right.impl_.data);
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

}  // namespace btck

/******************************************************************************/
// MARK: Block

namespace btck {

class Block
  : public detail::arc<BtcK_Block, BtcK_Block_Retain, BtcK_Block_Release>
  , public detail::range<Block const>
{
public:
  using value_type = Transaction;

  using base::base;

  Block(std::span<std::byte const> raw)
    : base{
        detail::invoke(BtcK_Block_New, raw.data(), raw.size()), detail::owned}
  {}

  [[nodiscard]] auto hash() const -> BlockHash
  {
    auto hash = BlockHash{};
    BtcK_Block_GetHash(this->impl(), &hash.impl_);
    return hash;
  }

  [[nodiscard]] auto size() const -> std::size_t
  {
    return BtcK_Block_NumTransactions(this->impl());
  }
  [[nodiscard]] auto operator[](std::size_t idx) const -> value_type
  {
    return {BtcK_Block_GetTransaction(this->impl(), idx), detail::owned};
  }

private:
  friend auto as_bytes(Block const& self) -> std::span<std::byte const>
  {
    auto len = std::size_t{};
    auto const* data = BtcK_Block_AsBytes(self.impl(), &len);
    return detail::as_bytes(data, len);
  }
};

enum class ValidationState : std::uint8_t {
  VALID,
  INVALID,
  ERROR,
};

enum class ValidationResult : std::uint8_t {
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

enum class chain_type : BtcK_ChainType {
  mainnet = BtcK_ChainType_MAINNET,
  testnet = BtcK_ChainType_TESTNET,
  testnet_4 = BtcK_ChainType_TESTNET_4,
  signet = BtcK_ChainType_SIGNET,
  regtest = BtcK_ChainType_REGTEST,
};

using Log = std::function<void(std::string_view)>;

enum class LogFlags : std::uint8_t {
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

template <typename T> class KernelNotifications
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

}  // namespace btck

/******************************************************************************/
// MARK: Chain

namespace btck {

class Chain : public detail::range<Chain const>
{
public:
  // poor man's cpp support for named arguments
  struct KwArgs {
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
    std::string_view data_dir, std::string_view blocks_dir, KwArgs kwargs = {});

  // auto ImportBlocks(std::span<std::string const> paths) -> bool;
  // auto ProcessBlock(Block const& block, bool* new_block) -> bool;

  using value_type = Block;

  [[nodiscard]] auto size() const -> std::size_t
  {
    return BtcK_Chain_NumBlocks(this->impl_.get());
  }

  [[nodiscard]] auto operator[](std::size_t height) const -> value_type
  {
    return {BtcK_Chain_GetBlock(this->impl_.get(), height), detail::owned};
  }

  [[nodiscard]] auto find(BlockHash const& block_hash) const -> iterator
  {
    std::ptrdiff_t const idx =
      BtcK_Chain_FindBlock(this->impl_.get(), &block_hash.impl_);
    return (idx == -1) ? this->end() : this->begin() + idx;
  }

private:
  struct deleter {
    void operator()(BtcK_Chain* chain) const { BtcK_Chain_Release(chain); }
  };

  std::unique_ptr<BtcK_Chain, deleter> impl_;
};

}  // namespace btck
