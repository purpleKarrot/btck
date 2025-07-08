// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <lauxlib.h>
#include <lua.h>
#include <string.h>

LUA_API void luaopen_btck(lua_State* L)
{
  static luaL_Reg const functions[] = {
    // {"version", btck_version},
    // {"get_balance", btck_get_balance},
    // {"send_transaction", btck_send_transaction},
    {NULL, NULL},
  };

  luaL_newlib(L, functions);
  lua_setglobal(L, "btck");
}
