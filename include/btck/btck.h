// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(_WIN32) || defined(__CYGWIN__)
#  define BTCK_IMPORT __declspec(dllimport)
#  define BTCK_EXPORT __declspec(dllexport)
#else
#  define BTCK_IMPORT __attribute__((visibility("default")))
#  define BTCK_EXPORT __attribute__((visibility("default")))
#endif

#ifdef btck_EXPORTS
#  define BTCK_API BTCK_EXPORT
#else
#  define BTCK_API BTCK_IMPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct btck_Block;
struct btck_Chain;
struct btck_ScriptPubkey;
struct btck_Transaction;
struct btck_TransactionOutput;

#define btck_BlockHash_SIZE 32
struct btck_BlockHash
{
  uint8_t data[btck_BlockHash_SIZE];
};

BTCK_API struct btck_ScriptPubkey* btck_ScriptPubkey_New(void const* raw, size_t len);
BTCK_API struct btck_ScriptPubkey* btck_ScriptPubkey_Retain(struct btck_ScriptPubkey* self);
BTCK_API void btck_ScriptPubkey_Release(struct btck_ScriptPubkey* self);
BTCK_API bool btck_ScriptPubkey_Equal(struct btck_ScriptPubkey const* left, struct btck_ScriptPubkey const* right);
BTCK_API void const* btck_ScriptPubkey_AsBytes(struct btck_ScriptPubkey const* self, size_t* len);

BTCK_API struct btck_TransactionOutput* btck_TransactionOutput_New(
  int64_t amount, struct btck_ScriptPubkey const* script_pubkey
);
BTCK_API struct btck_TransactionOutput* btck_TransactionOutput_Retain(struct btck_TransactionOutput* self);
BTCK_API void btck_TransactionOutput_Release(struct btck_TransactionOutput* self);
BTCK_API int64_t btck_TransactionOutput_GetAmount(struct btck_TransactionOutput const* self);
BTCK_API struct btck_ScriptPubkey* btck_TransactionOutput_GetScriptPubkey(struct btck_TransactionOutput const* self);

BTCK_API struct btck_Transaction* btck_Transaction_New(void const* raw, size_t len);
BTCK_API struct btck_Transaction* btck_Transaction_Retain(struct btck_Transaction* self);
BTCK_API void btck_Transaction_Release(struct btck_Transaction* self);
BTCK_API size_t btck_Transaction_GetSize(struct btck_Transaction const* self);
BTCK_API struct btck_TransactionOutput* btck_Transaction_At(struct btck_Transaction const* self, size_t idx);
BTCK_API void const* btck_Transaction_AsBytes(struct btck_Transaction const* self, size_t* len);
BTCK_API char const* btck_Transaction_ToString(struct btck_Transaction const* self, size_t* len);

// enum class VerificationError
// {
//   TX_INPUT_INDEX,
//   INVALID_FLAGS,
//   INVALID_FLAGS_COMBINATION,
//   SPENT_OUTPUTS_REQUIRED,
//   SPENT_OUTPUTS_MISMATCH,
// };

BTCK_API bool btck_Verify(
  struct btck_ScriptPubkey const* script_pubkey,
  int64_t amount,
  struct btck_Transaction const* tx_to,
  struct btck_TransactionOutput const* const* spent_outputs,
  size_t spent_outputs_len,
  unsigned int input_index,
  unsigned int flags,
  int* status
);

// auto verify(
//   ScriptPubkey const& script_pubkey,
//   std::int64_t amount,
//   Transaction const& tx_to,
//   std::span<TransactionOutput const> spent_outputs,
//   unsigned int input_index,
//   unsigned int flags
// ) -> std::optional<VerificationError>;

BTCK_API void btck_BlockHash_Init(struct btck_BlockHash* self, void const* raw, size_t len);

// auto as_bytes(BlockHash const& self) -> std::span<std::byte const, 32>;

BTCK_API struct btck_Block* btck_Block_New(void const* raw, size_t len);
BTCK_API struct btck_Block* btck_Block_Retain(struct btck_Block* self);
BTCK_API void btck_Block_Release(struct btck_Block* self);
BTCK_API void btck_Block_GetHash(struct btck_Block const* self, struct btck_BlockHash* out);
BTCK_API size_t btck_Block_GetSize(struct btck_Block const* self);
BTCK_API struct btck_Transaction* btck_Block_At(struct btck_Block const* self, size_t idx);
BTCK_API void const* btck_Block_AsBytes(struct btck_Block const* self, size_t* len);

// enum class ValidationState
// {
//   VALID,
//   INVALID,
//   ERROR,
// };

// enum class ValidationResult
// {
//   RESULT_UNSET,     //!< initial value. Block has not yet been rejected
//   CONSENSUS,        //!< invalid by consensus rules (excluding any below reasons)
//   CACHED_INVALID,   //!< this block was cached as being invalid and we didn't
//                     //!< store the reason why
//   INVALID_HEADER,   //!< invalid proof of work or time too old
//   MUTATED,          //!< the block's data didn't match the data committed to by the PoW
//   MISSING_PREV,     //!< We don't have the previous block the checked one is built
//                     //!< on
//   INVALID_PREV,     //!< A block this one builds on is invalid
//   TIME_FUTURE,      //!< block timestamp was > 2 hours in the future (or our clock is
//                     //!< bad)
//   HEADER_LOW_WORK,  //!< the block header may be on a too-little-work chain
// };

// using Validation = std::function<void(Block const&, ValidationState, ValidationResult)>;

// typedef is neededfor vapi!
//typedef double(*analyze_func)(int a, int b, void *userdata);

// typedef int(*compute_func)(int a, int b);
// typedef double(*analyze_func)(int a, int b, void *userdata);

// [CCode (cname = "compute_func", has_target = false)]
// public delegate int ComputeFunc (int a, int b);
// [CCode (cname = "analyze_func")]
// public delegate double AnalyzeFunc (int a, int b);

// enum class ChainType
// {
//   MAINNET = 0,
//   TESTNET,
//   TESTNET_4,
//   SIGNET,
//   REGTEST,
// };

// using Log = std::function<void(std::string_view)>;

// enum class LogFlags
// {
//   TIMESTAMPS,
//   TIME_MICROS,
//   THREADNAMES,
//   SOURCE_LOCATIONS,
//   CATEGORY_LEVELS,
// };

// class Logger
// {
// public:
//   Logger(Log log, LogFlags flags);
// };

// template <typename T>
// class KernelNotifications
// {
// public:
//   KernelNotifications();
//   // virtual void BlockTipHandler(kernel_SynchronizationState state, kernel_BlockIndex const* index) {}
//   // virtual void HeaderTipHandler(kernel_SynchronizationState state, int64_t height, int64_t timestamp, bool presync) {}
//   // virtual void ProgressHandler(std::string_view title, int progress_percent, bool resume_possible) {}
//   // virtual void WarningSetHandler(kernel_Warning warning, std::string_view message) {}
//   // virtual void WarningUnsetHandler(kernel_Warning warning) {}
//   // virtual void FlushErrorHandler(std::string_view error) {}
//   // virtual void FatalErrorHandler(std::string_view error) {}
// };

//   // poor man's cpp support for named arguments
//   struct KwArgs
//   {
//     KwArgs();
//     auto chain_type(ChainType arg) && -> KwArgs;
//     auto validation(Validation arg) && -> KwArgs;

//     template <typename T>
//     auto notifications(KernelNotifications<T> arg) && -> KwArgs;

//     auto SetWorkerThreads(int worker_threads) && -> KwArgs;
//     auto SetWipeDbs(bool wipe_block_tree, bool wipe_chainstate) && -> KwArgs;
//     auto SetBlockTreeDbInMemory(bool block_tree_db_in_memory) && -> KwArgs;
//     auto SetChainstateDbInMemory(bool chainstate_db_in_memory) && -> KwArgs;
//   };

//   Chain(std::string_view data_dir, std::string_view blocks_dir, KwArgs kwargs = {});
// BTCK_API struct btck_Chain* btck_Chain_New();

BTCK_API struct btck_Chain* btck_Chain_Retain(struct btck_Chain* self);
BTCK_API void btck_Chain_Release(struct btck_Chain* self);

BTCK_API size_t btck_Chain_GetSize(struct btck_Chain const* self);
BTCK_API struct btck_Block* btck_Chain_At(struct btck_Chain const* self, size_t idx);
BTCK_API ptrdiff_t btck_Chain_Find(struct btck_Chain const* self, struct btck_BlockHash const* block_hash);

//   bool ImportBlocks(std::span<std::string const> const paths) const;
//   bool ProcessBlock(Block const& block, bool* new_block) const;

#ifdef __cplusplus
}  // extern "C"
#endif
