// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__ELF__) || defined(__MACH__) || defined(__WASM__)
#  define BTCK_IMPORT __attribute__((visibility("default")))
#  define BTCK_EXPORT __attribute__((visibility("default")))
#else  // assume PE/COFF
#  define BTCK_IMPORT __declspec(dllimport)
#  define BTCK_EXPORT __declspec(dllexport)
#endif

#ifdef btck_EXPORTS
#  define BTCK_API BTCK_EXPORT
#else
#  define BTCK_API BTCK_IMPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct BtcK_Block;
struct BtcK_Chain;
struct BtcK_ScriptPubkey;
struct BtcK_Transaction;
struct BtcK_TransactionOutput;

struct BtcK_Error;

BTCK_API struct BtcK_Error* BtcK_Error_New(
  char const* domain, int code, char const* message
);

BTCK_API void BtcK_Error_Free(struct BtcK_Error* error);

BTCK_API int BtcK_Error_Code(struct BtcK_Error const* error);
BTCK_API char const* BtcK_Error_Domain(struct BtcK_Error const* error);
BTCK_API char const* BtcK_Error_Message(struct BtcK_Error const* error);

/*****************************************************************************/

#define BtcK_BlockHash_SIZE 32
struct BtcK_BlockHash
{
  unsigned char data[BtcK_BlockHash_SIZE];
};

/*****************************************************************************/

BTCK_API struct BtcK_ScriptPubkey* BtcK_ScriptPubkey_New(
  void const* raw, size_t len, struct BtcK_Error** err
);

BTCK_API struct BtcK_ScriptPubkey* BtcK_ScriptPubkey_Retain(
  struct BtcK_ScriptPubkey* self
);

BTCK_API void BtcK_ScriptPubkey_Release(struct BtcK_ScriptPubkey* self);

BTCK_API bool BtcK_ScriptPubkey_Equal(
  struct BtcK_ScriptPubkey const* left, struct BtcK_ScriptPubkey const* right
);

BTCK_API void const* BtcK_ScriptPubkey_AsBytes(
  struct BtcK_ScriptPubkey const* self, size_t* len
);

/*****************************************************************************/

BTCK_API struct BtcK_TransactionOutput* BtcK_TransactionOutput_New(
  int64_t amount,
  struct BtcK_ScriptPubkey const* script_pubkey,
  struct BtcK_Error** err
);

BTCK_API struct BtcK_TransactionOutput* BtcK_TransactionOutput_Retain(
  struct BtcK_TransactionOutput* self
);

BTCK_API void BtcK_TransactionOutput_Release(
  struct BtcK_TransactionOutput* self
);

BTCK_API int64_t
BtcK_TransactionOutput_GetAmount(struct BtcK_TransactionOutput const* self);

BTCK_API struct BtcK_ScriptPubkey* BtcK_TransactionOutput_GetScriptPubkey(
  struct BtcK_TransactionOutput const* self, struct BtcK_Error** err
);

/*****************************************************************************/

BTCK_API struct BtcK_Transaction* BtcK_Transaction_New(
  void const* raw, size_t len, struct BtcK_Error** err
);

BTCK_API struct BtcK_Transaction* BtcK_Transaction_Retain(
  struct BtcK_Transaction* self
);

BTCK_API void BtcK_Transaction_Release(struct BtcK_Transaction* self);

BTCK_API size_t
BtcK_Transaction_NumOutputs(struct BtcK_Transaction const* self);

BTCK_API struct BtcK_TransactionOutput* BtcK_Transaction_GetOutput(
  struct BtcK_Transaction const* self, size_t idx
);

BTCK_API void const* BtcK_Transaction_AsBytes(
  struct BtcK_Transaction const* self, size_t* len
);

BTCK_API char const* BtcK_Transaction_ToString(
  struct BtcK_Transaction const* self, size_t* len
);

/*****************************************************************************/

typedef uint8_t BtcK_VerificationError;

#define BtcK_VerificationError_TX_INPUT_INDEX ((BtcK_VerificationError)(1))

#define BtcK_VerificationError_INVALID_FLAGS ((BtcK_VerificationError)(2))

#define BtcK_VerificationError_INVALID_FLAGS_COMBINATION                       \
  ((BtcK_VerificationError)(3))

#define BtcK_VerificationError_SPENT_OUTPUTS_REQUIRED                          \
  ((BtcK_VerificationError)(4))

#define BtcK_VerificationError_SPENT_OUTPUTS_MISMATCH                          \
  ((BtcK_VerificationError)(5))

BTCK_API char const* BtcK_VerificationError_Message(BtcK_VerificationError err);

/*****************************************************************************/

typedef uint32_t BtcK_VerificationFlags;

#define BtcK_VerificationFlags_NONE ((BtcK_VerificationFlags)(0))

#define BtcK_VerificationFlags_P2SH ((BtcK_VerificationFlags)(1U << 0))

#define BtcK_VerificationFlags_DERSIG ((BtcK_VerificationFlags)(1U << 2))

#define BtcK_VerificationFlags_NULLDUMMY ((BtcK_VerificationFlags)(1U << 4))

#define BtcK_VerificationFlags_CHECKLOCKTIMEVERIFY                             \
  ((BtcK_VerificationFlags)(1U << 9))

#define BtcK_VerificationFlags_CHECKSEQUENCEVERIFY                             \
  ((BtcK_VerificationFlags)(1U << 10))

#define BtcK_VerificationFlags_WITNESS ((BtcK_VerificationFlags)(1U << 11))

#define BtcK_VerificationFlags_TAPROOT ((BtcK_VerificationFlags)(1U << 17))

#define BtcK_VerificationFlags_ALL                                             \
  ((BtcK_VerificationFlags)(BtcK_VerificationFlags_P2SH |                      \
                            BtcK_VerificationFlags_DERSIG |                    \
                            BtcK_VerificationFlags_NULLDUMMY |                 \
                            BtcK_VerificationFlags_CHECKLOCKTIMEVERIFY |       \
                            BtcK_VerificationFlags_CHECKSEQUENCEVERIFY |       \
                            BtcK_VerificationFlags_WITNESS |                   \
                            BtcK_VerificationFlags_TAPROOT))

BTCK_API int BtcK_VerificationFlags_ToString(
  BtcK_VerificationFlags flags, char* buf, size_t len
);

BTCK_API bool BtcK_Verify(
  struct BtcK_ScriptPubkey const* script_pubkey,
  int64_t amount,
  struct BtcK_Transaction const* tx_to,
  struct BtcK_TransactionOutput const* const* spent_outputs,
  size_t spent_outputs_len,
  unsigned int input_index,
  BtcK_VerificationFlags flags,
  struct BtcK_Error** err
);

/*****************************************************************************/

BTCK_API void BtcK_BlockHash_Init(
  struct BtcK_BlockHash* self, void const* raw, size_t len
);

// auto as_bytes(BlockHash const& self) -> std::span<std::byte const, 32>;

/*****************************************************************************/

BTCK_API struct BtcK_Block* BtcK_Block_New(
  void const* raw, size_t len, struct BtcK_Error** err
);

BTCK_API struct BtcK_Block* BtcK_Block_Retain(struct BtcK_Block* self);

BTCK_API void BtcK_Block_Release(struct BtcK_Block* self);

BTCK_API void BtcK_Block_GetHash(
  struct BtcK_Block const* self, struct BtcK_BlockHash* out
);

BTCK_API size_t BtcK_Block_NumTransactions(struct BtcK_Block const* self);

BTCK_API struct BtcK_Transaction* BtcK_Block_GetTransaction(
  struct BtcK_Block const* self, size_t idx
);

BTCK_API void const* BtcK_Block_AsBytes(
  struct BtcK_Block const* self, size_t* len
);

/*****************************************************************************/

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

typedef uint8_t BtcK_ChainType;
#define BtcK_ChainType_MAINNET (BtcK_ChainType(0))
#define BtcK_ChainType_TESTNET (BtcK_ChainType(1))
#define BtcK_ChainType_TESTNET_4 (BtcK_ChainType(2))
#define BtcK_ChainType_SIGNET (BtcK_ChainType(3))
#define BtcK_ChainType_REGTEST (BtcK_ChainType(4))

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
// BTCK_API BtcK_Chain* BtcK_Chain_New();

BTCK_API struct BtcK_Chain* BtcK_Chain_Retain(struct BtcK_Chain* self);
BTCK_API void BtcK_Chain_Release(struct BtcK_Chain* self);

BTCK_API size_t BtcK_Chain_NumBlocks(struct BtcK_Chain const* self);
BTCK_API struct BtcK_Block* BtcK_Chain_GetBlock(
  struct BtcK_Chain const* self, size_t idx
);
BTCK_API ptrdiff_t BtcK_Chain_FindBlock(
  struct BtcK_Chain const* self, struct BtcK_BlockHash const* block_hash
);

//   bool ImportBlocks(std::span<std::string const> const paths) const;
//   bool ProcessBlock(Block const& block, bool* new_block) const;

#ifdef __cplusplus
}  // extern "C"
#endif
