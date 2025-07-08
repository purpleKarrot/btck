// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <lauxlib.h>
#include <lua.h>
#include <stdlib.h>
#include <string.h>

#include <btck/btck.h>

struct Self
{
  struct btck_Block* impl;
};
