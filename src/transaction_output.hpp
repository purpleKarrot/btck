// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <primitives/transaction.h>

#include <utility>

#include "consensus/amount.h"
#include "script/script.h"
#include "util/ref_counted.hpp"

struct BtcK_TransactionOutput : util::RefCounted<BtcK_TransactionOutput> {
  BtcK_TransactionOutput(CTxOut arg)
    : tx_out{std::move(arg)}
  {}

  BtcK_TransactionOutput(CAmount amount, CScript script_pubkey)
    : tx_out{amount, std::move(script_pubkey)}
  {}

  CTxOut tx_out;
};
