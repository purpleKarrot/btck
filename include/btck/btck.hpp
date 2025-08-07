// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <btck/btck.h>  // IWYU pragma: export

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iterator>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

struct BtcK_Block;
struct BtcK_Chain;
struct BtcK_Error;
struct BtcK_ScriptPubkey;
struct BtcK_Transaction;
struct BtcK_TransactionOutput;

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
// MARK: Ownership

namespace btck::detail {

template <typename T> struct c_api_traits;

template <typename T> struct owned_policy {
  using pointer = T*;
  using const_pointer = T const*;
  static auto copy(T const* p) { return c_api_traits<T>::copy(p); }
  static void free(T* p) { c_api_traits<T>::free(p); }
};

template <typename T> struct unowned_policy {
  using pointer = T const*;
  using const_pointer = T const*;
  static auto copy(T const* p) { return p; }
  static void free(T const* /*p*/) {}
};

struct internal_t {};
constexpr auto const internal = internal_t{};

template <template <typename> typename Api, template <typename> typename Owned>
class wrapper : public Api<wrapper<Api, Owned>>
{
  using ownership_traits = Owned<typename Api<wrapper>::c_type>;

public:
  using base = wrapper;

  wrapper(internal_t /*internal*/, ownership_traits::pointer ptr)
    : ptr_(ptr)
  {}

  wrapper(wrapper const& other)
    : ptr_(ownership_traits::copy(other.ptr_))
  {}

  wrapper(wrapper&& other) noexcept
    : ptr_(other.ptr_)
  {
    other.ptr_ = nullptr;
  }

  template <template <typename> class OtherOwned>
  wrapper(wrapper<Api, OtherOwned> const& other)
    : ptr_(ownership_traits::copy(other.get()))
  {}

  ~wrapper() { ownership_traits::free(ptr_); }

  auto operator=(wrapper const& other) -> wrapper&
  {
    std::swap(ptr_, wrapper(other).ptr_);
    return *this;
  }

  auto operator=(wrapper&& other) noexcept -> wrapper&
  {
    ownership_traits::free(ptr_);
    ptr_ = std::exchange(other.ptr_, nullptr);
    return *this;
  }

  [[nodiscard]] auto get() -> ownership_traits::pointer { return ptr_; }

  [[nodiscard]] auto get() const -> ownership_traits::const_pointer
  {
    return ptr_;
  }

private:
  friend struct get_impl_;
  ownership_traits::pointer ptr_;
};

template <template <typename> typename Api>
auto make_unowned(wrapper<Api, owned_policy> const& arg)
  -> wrapper<Api, unowned_policy>;

struct get_impl_ {
  template <template <class> class Api, template <class> class Owned>
  auto operator()(wrapper<Api, Owned>& arg) const
  {
    return arg.ptr_;
  }

  template <template <class> class Api, template <class> class Owned>
  auto operator()(wrapper<Api, Owned> const& arg) const
  {
    return arg.ptr_;
  }

  template <template <class> class Api, template <class> class Owned>
  auto operator()(wrapper<Api, Owned> const* arg) const
  {
    return &arg->ptr_;
  }
};

constexpr auto const get_impl = get_impl_{};

}  // namespace btck::detail

namespace btck {

template <typename T>
using unowned = decltype(detail::make_unowned(std::declval<T>()));

}  // namespace btck

/******************************************************************************/
// MARK: to_string

namespace btck::detail {

using to_bytes_fn = int (*)(void const*, BtcK_WriteBytes, void*);
auto to_bytes_(void const* obj, to_bytes_fn writefn) -> std::vector<std::byte>;

using to_string_fn = int (*)(void const*, char*, size_t);
auto to_string_(void const* obj, to_string_fn printfn) -> std::string;

template <typename T>
auto to_bytes(T const* obj, int (*writefn)(T const*, BtcK_WriteBytes, void*))
{
  return to_bytes_(
    reinterpret_cast<void const*>(obj), reinterpret_cast<to_bytes_fn>(writefn));
}

template <typename T>
auto to_string(T const* obj, int (*printfn)(T const*, char*, size_t))
{
  return to_string_(
    reinterpret_cast<void const*>(obj),
    reinterpret_cast<to_string_fn>(printfn));
}

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
  return detail::to_string(
    &cflags, +[](BtcK_VerificationFlags const* f, char* buf, size_t len) {
      return BtcK_VerificationFlags_ToString(*f, buf, len);
    });
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

[[noreturn]] void translate_error(error const& err);

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

template <> struct btck::detail::c_api_traits<BtcK_ScriptPubkey> {
  static auto copy(BtcK_ScriptPubkey const* self)
  {
    return invoke(BtcK_ScriptPubkey_Copy, self);
  }

  static void free(BtcK_ScriptPubkey* self) { BtcK_ScriptPubkey_Free(self); }
};

namespace btck {

class transaction;
class transaction_output;

namespace detail {

template <typename Derived> class script_pubkey_api
{
public:
  using c_type = BtcK_ScriptPubkey;

  [[nodiscard]] auto verify(
    std::int64_t amount, transaction const& tx_to,
    std::span<transaction_output const> spent_outputs, unsigned int input_index,
    verification_flags flags) const -> bool;

private:
  friend auto to_bytes(script_pubkey_api const& self) -> std::vector<std::byte>
  {
    return detail::to_bytes(self.impl(), BtcK_ScriptPubkey_ToBytes);
  }

  [[nodiscard]] auto impl() const
  {
    return static_cast<Derived const*>(this)->get();
  }

  friend Derived;
  script_pubkey_api() = default;
};

template <template <class> class LeftOwned, template <class> class RightOwned>
auto operator==(
  wrapper<script_pubkey_api, LeftOwned> const& left,
  wrapper<script_pubkey_api, RightOwned> const& right) -> bool
{
  return BtcK_ScriptPubkey_Equal(get_impl(left), get_impl(right)) != 0;
}

}  // namespace detail

class script_pubkey
  : public detail::wrapper<detail::script_pubkey_api, detail::owned_policy>
{
public:
  using base::base;

  script_pubkey(std::span<std::byte const> raw)
    : base{
        detail::internal,
        detail::invoke(BtcK_ScriptPubkey_New, raw.data(), raw.size())}
  {}
};

}  // namespace btck

/******************************************************************************/
// MARK: TransactionOutput

template <> struct btck::detail::c_api_traits<BtcK_TransactionOutput> {
  static auto copy(BtcK_TransactionOutput const* self)
  {
    return invoke(BtcK_TransactionOutput_Copy, self);
  }

  static void free(BtcK_TransactionOutput* self)
  {
    BtcK_TransactionOutput_Free(self);
  }
};

namespace btck {
namespace detail {

template <typename Derived> class transaction_output_api
{
public:
  using c_type = BtcK_TransactionOutput;

  [[nodiscard]] auto amount() const -> std::int64_t
  {
    return BtcK_TransactionOutput_GetAmount(this->impl());
  }

  [[nodiscard]] auto script_pubkey() const -> unowned<btck::script_pubkey>
  {
    return {
      detail::internal, BtcK_TransactionOutput_GetScriptPubkey(this->impl())};
  }

private:
  friend auto to_string(transaction_output_api const& self)
  {
    return detail::to_string(self.impl(), BtcK_TransactionOutput_ToString);
  }

  [[nodiscard]] auto impl() const
  {
    return static_cast<Derived const*>(this)->get();
  }

  friend Derived;
  transaction_output_api() = default;
};

}  // namespace detail

class transaction_output
  : public detail::wrapper<detail::transaction_output_api, detail::owned_policy>
{
public:
  using base::base;

  transaction_output(std::int64_t amount, btck::script_pubkey const& sp)
    : base{
        detail::internal,
        detail::invoke(
          BtcK_TransactionOutput_New, amount, detail::get_impl(sp))}
  {}
};

}  // namespace btck

/******************************************************************************/
// MARK: Transaction

template <> struct btck::detail::c_api_traits<BtcK_Transaction> {
  static auto copy(BtcK_Transaction const* self)
  {
    return invoke(BtcK_Transaction_Copy, self);
  }

  static void free(BtcK_Transaction* self) { BtcK_Transaction_Free(self); }
};

namespace btck {
namespace detail {

template <typename Derived>
class transaction_outputs_api
  : public range<transaction_outputs_api<Derived> const>
{
public:
  using c_type = BtcK_Transaction const;
  using value_type = unowned<transaction_output>;

  [[nodiscard]] auto size() const -> std::size_t
  {
    return BtcK_Transaction_CountOutputs(this->impl());
  }

  [[nodiscard]] auto operator[](std::size_t idx) const -> value_type
  {
    return {
      detail::internal,
      BtcK_Transaction_GetOutput(this->impl(), idx),
    };
  }

private:
  [[nodiscard]] auto impl() const
  {
    return static_cast<Derived const*>(this)->get();
  }

  friend Derived;
  transaction_outputs_api() = default;
};

template <typename Derived> class transaction_api
{
public:
  using c_type = BtcK_Transaction;

  [[nodiscard]] auto outputs() const
    -> detail::wrapper<transaction_outputs_api, unowned_policy>
  {
    return {detail::internal, impl()};
  }

private:
  friend auto to_bytes(transaction_api const& self) -> std::vector<std::byte>
  {
    return detail::to_bytes(self.impl(), BtcK_Transaction_ToBytes);
  }

  friend auto to_string(transaction_api const& self)
  {
    return detail::to_string(self.impl(), BtcK_Transaction_ToString);
  }

  [[nodiscard]] auto impl() const
  {
    return static_cast<Derived const*>(this)->get();
  }

  friend Derived;
  transaction_api() = default;
};

}  // namespace detail

class transaction
  : public detail::wrapper<detail::transaction_api, detail::owned_policy>
{
public:
  using base::base;

  transaction(std::span<std::byte const> raw)
    : base{
        detail::internal,
        detail::invoke(BtcK_Transaction_New, raw.data(), raw.size())}
  {}
};

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

public:
  BtcK_BlockHash impl_;
};

}  // namespace btck

/******************************************************************************/
// MARK: Block

template <> struct btck::detail::c_api_traits<BtcK_Block> {
  static auto copy(BtcK_Block const* self)
  {
    return invoke(BtcK_Block_Copy, self);
  }

  static void free(BtcK_Block* self) { BtcK_Block_Free(self); }
};

namespace btck {
namespace detail {

template <typename Derived>
class block_transactions_api
  : public range<block_transactions_api<Derived> const>
{
public:
  using c_type = BtcK_Block const;
  using value_type = unowned<transaction>;

  [[nodiscard]] auto size() const -> std::size_t
  {
    return BtcK_Block_CountTransactions(this->impl());
  }

  [[nodiscard]] auto operator[](std::size_t idx) const -> value_type
  {
    return {
      detail::internal,
      BtcK_Block_GetTransaction(this->impl(), idx),
    };
  }

private:
  [[nodiscard]] auto impl() const
  {
    return static_cast<Derived const*>(this)->get();
  }

  friend Derived;
  block_transactions_api() = default;
};

template <typename Derived> class block_api
{
public:
  using c_type = BtcK_Block;

  [[nodiscard]] auto hash() const -> BlockHash
  {
    auto hash = BlockHash{};
    BtcK_Block_GetHash(this->impl(), &hash.impl_);
    return hash;
  }

  [[nodiscard]] auto transactions() const
    -> detail::wrapper<block_transactions_api, unowned_policy>
  {
    return {detail::internal, impl()};
  }

private:
  friend auto to_bytes(block_api const& self) -> std::vector<std::byte>
  {
    return detail::to_bytes(self.impl(), BtcK_Block_ToBytes);
  }

  friend auto to_string(block_api const& self)
  {
    return detail::to_string(self.impl(), BtcK_Block_ToString);
  }

  [[nodiscard]] auto impl() const
  {
    return static_cast<Derived const*>(this)->get();
  }

  friend Derived;
  block_api() = default;
};

}  // namespace detail

class block : public detail::wrapper<detail::block_api, detail::owned_policy>
{
public:
  using api_policy = block_api;
  using base::base;

  block(std::span<std::byte const> raw)
    : base{
        detail::internal,
        detail::invoke(BtcK_Block_New, raw.data(), raw.size())}
  {}
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
  std::function<void(unowned<block> const&, ValidationState, ValidationResult)>;

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

  using value_type = block;

  [[nodiscard]] auto size() const -> std::size_t
  {
    return BtcK_Chain_CountBlocks(this->impl_.get());
  }

  [[nodiscard]] auto operator[](std::size_t height) const -> value_type
  {
    return {
      detail::internal,
      detail::invoke(BtcK_Chain_GetBlock, this->impl_.get(), height),
    };
  }

  [[nodiscard]] auto find(BlockHash const& block_hash) const -> iterator
  {
    std::ptrdiff_t const idx =
      BtcK_Chain_FindBlock(this->impl_.get(), &block_hash.impl_);
    return (idx == -1) ? this->end() : this->begin() + idx;
  }

private:
  struct deleter {
    void operator()(BtcK_Chain* chain) const { BtcK_Chain_Free(chain); }
  };

  std::unique_ptr<BtcK_Chain, deleter> impl_;
};

}  // namespace btck

/******************************************************************************/
// MARK: Implementations

template <typename Derived>
auto btck::detail::script_pubkey_api<Derived>::verify(
  std::int64_t amount, transaction const& tx_to,
  std::span<transaction_output const> spent_outputs, unsigned int input_index,
  verification_flags flags) const -> bool
{
  int const result = detail::invoke(
    BtcK_ScriptPubkey_Verify, this->impl(), amount, detail::get_impl(tx_to),
    (spent_outputs.empty() ? nullptr : detail::get_impl(spent_outputs.data())),
    spent_outputs.size(), input_index,
    static_cast<BtcK_VerificationFlags>(flags));
  return result != 0;
}
