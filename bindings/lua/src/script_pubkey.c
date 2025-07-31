// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <btck/btck.h>

#include <lauxlib.h>
#include <lua.h>
#include <stdlib.h>
#include <string.h>

struct Self {
  struct btck_ScriptPubkey* c;
  char* name;
};

static int lcounter_new(lua_State* L)
{
  lua_Integer start = luaL_checkinteger(L, 1);
  char const* name = luaL_checkstring(L, 2);
  if (name == NULL) {
    luaL_error(L, "name cannot be empty");
  }

  struct Self* cu = lua_newuserdata(L, sizeof(struct Self));
  cu->c = NULL;
  cu->name = NULL;

  luaL_getmetatable(L, "LCounter");
  lua_setmetatable(L, -2);

  /* Create the data that comprises the userdata (the counter). */
  cu->c = btck_ScriptPubkey_New(NULL, 0);
  cu->name = strdup(name);

  return 1;
}

static int lcounter_add(lua_State* L)
{
  struct Self* cu = luaL_checkudata(L, 1, "LCounter");
  lua_Integer amount = luaL_checkinteger(L, 2);
  counter_add(cu->c, amount);

  return 0;
}

static int lcounter_subtract(lua_State* L)
{
  struct Self* cu = luaL_checkudata(L, 1, "LCounter");
  lua_Integer amount = luaL_checkinteger(L, 2);
  counter_subtract(cu->c, amount);

  return 0;
}

static int lcounter_increment(lua_State* L)
{
  struct Self* cu = luaL_checkudata(L, 1, "LCounter");
  counter_increment(cu->c);

  return 0;
}

static int lcounter_decrement(lua_State* L)
{
  struct Self* cu = luaL_checkudata(L, 1, "LCounter");
  counter_decrement(cu->c);

  return 0;
}

static int lcounter_getval(lua_State* L)
{
  struct Self* cu = luaL_checkudata(L, 1, "LCounter");
  lua_pushinteger(L, counter_getval(cu->c));

  return 1;
}

static int lcounter_getname(lua_State* L)
{
  struct Self* cu = luaL_checkudata(L, 1, "LCounter");
  lua_pushstring(L, cu->name);

  return 1;
}

static int lcounter_destroy(lua_State* L)
{
  struct Self* cu = luaL_checkudata(L, 1, "LCounter");

  if (cu->c != NULL) {
    counter_destroy(cu->c);
  }

  cu->c = NULL;

  if (cu->name != NULL) {
    free(cu->name);
  }

  cu->name = NULL;

  return 0;
}

static int lcounter_tostring(lua_State* L)
{
  struct Self* cu = luaL_checkudata(L, 1, "LCounter");

  lua_pushfstring(L, "%s(%d)", cu->name, counter_getval(cu->c));

  return 1;
}

static const struct luaL_Reg lcounter_methods[] = {
  {"add", lcounter_add},
  {"subtract", lcounter_subtract},
  {"increment", lcounter_increment},
  {"decrement", lcounter_decrement},
  {"getval", lcounter_getval},
  {"getname", lcounter_getname},
  {"__gc", lcounter_destroy},
  {"__tostring", lcounter_tostring},
  {NULL, NULL},
};

static const struct luaL_Reg lcounter_functions[] = {
  {"new", lcounter_new},
  {NULL, NULL},
};

int luaopen_lcounter(lua_State* L)
{
  /* Create the metatable and put it on the stack. */
  luaL_newmetatable(L, "LCounter");
  /* Duplicate the metatable on the stack (We know have 2). */
  lua_pushvalue(L, -1);
  /* Pop the first metatable off the stack and assign it to __index
     * of the second one. We set the metatable for the table to itself.
     * This is equivalent to the following in lua:
     * metatable = {}
     * metatable.__index = metatable
     */
  lua_setfield(L, -2, "__index");

  /* Set the methods to the metatable that should be accessed via object:func */
  luaL_setfuncs(L, lcounter_methods, 0);

  /* Register the object.func functions into the table that is at the top of the
     * stack. */
  luaL_newlib(L, lcounter_functions);

  return 1;
}
