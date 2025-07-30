// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <btck/btck.h>

char const* BtcK_VerificationError_Message(BtcK_VerificationError error)
{
  switch (error) {
    case BtcK_VerificationError_TX_INPUT_INDEX:
      return "The provided input index is out of range of the actual number of "
             "inputs of the transaction.";

    case BtcK_VerificationError_INVALID_FLAGS:
      return "The provided bitfield for the flags was invalid.";

    case BtcK_VerificationError_INVALID_FLAGS_COMBINATION:
      return "The flags very combined in an invalid way.";

    case BtcK_VerificationError_SPENT_OUTPUTS_REQUIRED:
      return "The taproot flag was set, so valid spent_outputs have to be "
             "provided.";

    case BtcK_VerificationError_SPENT_OUTPUTS_MISMATCH:
      return "The number of spent outputs does not match the number of inputs "
             "of the tx.";

    default:
      return "(unrecognized error)";
  }
}
