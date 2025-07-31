// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "script_pubkey.hpp"

#include <btck/btck.h>

#include <cstddef>
#include <cstdint>
#include <span>

#include "script/script.h"
#include "util/as_bytes.hpp"
#include "util/error.hpp"

extern "C" {

auto BtcK_ScriptPubkey_New(
  void const* raw, std::size_t len, struct BtcK_Error** err)
  -> BtcK_ScriptPubkey*
{
  return util::WrapFn(err, [=] {
    return new BtcK_ScriptPubkey{
      std::span{reinterpret_cast<std::uint8_t const*>(raw), len}};
  });
}

auto BtcK_ScriptPubkey_Retain(BtcK_ScriptPubkey* self) -> BtcK_ScriptPubkey*
{
  return self->Retain();
}

void BtcK_ScriptPubkey_Release(BtcK_ScriptPubkey* self)
{
  self->Release();
}

auto BtcK_ScriptPubkey_Equal(
  BtcK_ScriptPubkey const* left, BtcK_ScriptPubkey const* right) -> bool
{
  return left->script == right->script;
}

auto BtcK_ScriptPubkey_AsBytes(BtcK_ScriptPubkey const* self, std::size_t* len)
  -> void const*
{
  return util::AsBytes(self->script, len);
}

}  // extern "C"
