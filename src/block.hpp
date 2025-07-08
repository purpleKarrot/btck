// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <span>

#include <btck/btck.h>
#include <primitives/block.h>

#include "util/ref_counted.hpp"

struct btck_Block : util::RefCounted<btck_Block>
{
  btck_Block(std::span<std::byte const> raw);

  CBlock block;
};
