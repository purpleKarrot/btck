// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <span>

#include <btck/btck.h>
#include <script/script.h>

#include "util/ref_counted.hpp"

struct btck_ScriptPubkey : util::RefCounted<btck_ScriptPubkey>
{
  btck_ScriptPubkey(std::span<std::uint8_t const> raw)
    : script(raw.begin(), raw.end())
  {}

  CScript script;
};
