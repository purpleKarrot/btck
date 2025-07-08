// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <btck/btck.h>
#include <primitives/transaction.h>

#include "util/ref_counted.hpp"

struct btck_TransactionOutput : util::RefCounted<btck_TransactionOutput>
{
  btck_TransactionOutput(CTxOut arg)
    : tx_out{std::move(arg)}
  {}

  btck_TransactionOutput(CAmount amount, CScript script_pubkey)
    : tx_out{amount, std::move(script_pubkey)}
  {}

  CTxOut tx_out;
};
