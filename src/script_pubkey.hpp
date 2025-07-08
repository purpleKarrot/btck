// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <cstdint>
#include <span>

#include <script/script.h>

#include "util/ref_counted.hpp"

struct BtcK_ScriptPubkey : util::RefCounted<BtcK_ScriptPubkey>
{
  BtcK_ScriptPubkey(std::span<std::uint8_t const> raw)
    : script(raw.begin(), raw.end())
  {}

  CScript script;
};
