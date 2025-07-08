// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <span>

#include <btck/btck.h>
#include <primitives/transaction.h>

#include "util/ref_counted.hpp"

struct btck_Transaction : util::RefCounted<btck_Transaction>
{
  btck_Transaction(CTransactionRef arg);
  btck_Transaction(std::span<std::byte const> raw);

  CTransactionRef transaction;
  std::vector<std::byte> serialized;
  std::string string;
};
