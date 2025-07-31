// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <primitives/block.h>

#include <cstddef>
#include <span>

#include "util/ref_counted.hpp"

namespace node {

class BlockManager;

}  // namespace node

class CBlockIndex;

struct BtcK_Block : util::RefCounted<BtcK_Block> {
  BtcK_Block(std::span<std::byte const> raw);
  BtcK_Block(CBlockIndex const& bi, node::BlockManager const& bm);

  CBlock block;
};
