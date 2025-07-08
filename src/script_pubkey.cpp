// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "script_pubkey.hpp"

#include <cstddef>
#include <cstdint>
#include <span>

#include "util/as_bytes.hpp"

extern "C" {

auto btck_ScriptPubkey_New(void const* raw, size_t len) -> btck_ScriptPubkey*
{
  return btck_ScriptPubkey::New(std::span{reinterpret_cast<std::uint8_t const*>(raw), len});
}

auto btck_ScriptPubkey_Retain(btck_ScriptPubkey* self) -> btck_ScriptPubkey*
{
  return self->Retain();
}

void btck_ScriptPubkey_Release(btck_ScriptPubkey* self)
{
  self->Release();
}

auto btck_ScriptPubkey_Equal(struct btck_ScriptPubkey const* left, struct btck_ScriptPubkey const* right) -> bool
{
  return left->script == right->script;
}

auto btck_ScriptPubkey_AsBytes(btck_ScriptPubkey const* self, size_t* len) -> void const*
{
  return util::AsBytes(self->script, len);
}

}  // extern "C"
