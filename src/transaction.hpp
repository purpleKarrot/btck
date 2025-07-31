// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <primitives/transaction.h>

#include <cstddef>
#include <span>
#include <string>
#include <vector>

#include "util/ref_counted.hpp"

struct BtcK_Transaction : util::RefCounted<BtcK_Transaction> {
  BtcK_Transaction(CTransactionRef arg);
  BtcK_Transaction(std::span<std::byte const> raw);

  CTransactionRef transaction;
  std::vector<std::byte> serialized;
  std::string string;
};
