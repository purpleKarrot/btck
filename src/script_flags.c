// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include <btck/btck.h>

struct PrintContext
{
  char* buffer;
  size_t bufsz;
  int num;
};

static int print(struct PrintContext* ctx, char const* format, ...)
{
  va_list args;
  va_start(args, format);
  int retval = vsnprintf(ctx->buffer, ctx->bufsz, format, args);
  va_end(args);
  if (retval < 0) {
    return retval;
  }

  ctx->buffer = (ctx->buffer != NULL) ? ctx->buffer + (size_t)retval : NULL;
  ctx->bufsz = ((size_t)retval < ctx->bufsz) ? ctx->bufsz - (size_t)retval : 0;
  return ctx->num += retval;
}

struct Entry
{
  BtcK_ScriptVerify flag;
  char const* name;
};

static struct Entry const flag_table[] = {
  {BtcK_ScriptVerify_P2SH, "P2SH"},
  {BtcK_ScriptVerify_DERSIG, "DERSIG"},
  {BtcK_ScriptVerify_NULLDUMMY, "NULLDUMMY"},
  {BtcK_ScriptVerify_CHECKLOCKTIMEVERIFY, "CHECKLOCKTIMEVERIFY"},
  {BtcK_ScriptVerify_CHECKSEQUENCEVERIFY, "CHECKSEQUENCEVERIFY"},
  {BtcK_ScriptVerify_WITNESS, "WITNESS"},
  {BtcK_ScriptVerify_TAPROOT, "TAPROOT"},
};

int BtcK_ScriptVerify_ToString(BtcK_ScriptVerify flags, char* buf, size_t len)
{
  if (flags == BtcK_ScriptVerify_ALL) {
    return snprintf(buf, len, "ALL");
  }

  if (flags == BtcK_ScriptVerify_NONE) {
    return snprintf(buf, len, "NONE");
  }

  if ((flags & ~BtcK_ScriptVerify_ALL) != 0) {
    return -1;
  }

  struct PrintContext ctx = {buf, len, 0};
  bool first = true;

  size_t const nflags = sizeof(flag_table) / sizeof(flag_table[0]);
  for (size_t i = 0; i < nflags; ++i) {
    if (flags & flag_table[i].flag) {
      int retval = print(&ctx, "%s%s", first ? "" : " | ", flag_table[i].name);
      if (retval < 0) {
        return retval;
      }
      first = false;
    }
  }

  return ctx.num;
}
