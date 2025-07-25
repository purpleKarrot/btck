// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "util/ref_counted.hpp"
#include "validation.h"

struct BtcK_Chain : util::RefCounted<BtcK_Chain>
{
  ChainstateManager chainstate_manager;
};
