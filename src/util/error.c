// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <stdlib.h>
#include <string.h>

#include <btck/btck.h>

struct BtcK_Error
{
  char* domain;
  char* message;
  int code;
};

static struct BtcK_Error OOM_ERROR = {
  .domain = "Memory",
  .message = "Out of memory",
  .code = -1,
};

struct BtcK_Error* BtcK_Error_New(
  char const* domain, int code, char const* message
)
{
  struct BtcK_Error* err = malloc(sizeof(struct BtcK_Error));
  if (err == NULL) {
    return &OOM_ERROR;
  }

  err->domain = domain ? strdup(domain) : NULL;
  err->message = message ? strdup(message) : NULL;
  err->code = code;

  if ((domain && !err->domain) || (message && !err->message)) {
    BtcK_Error_Free(err);
    return &OOM_ERROR;
  }

  return err;
}

void BtcK_Error_Free(struct BtcK_Error* error)
{
  if (error == NULL || error == &OOM_ERROR) {
    return;
  }

  free(error->domain);
  free(error->message);
  free(error);
}

int BtcK_Error_Code(struct BtcK_Error const* error)
{
  return error->code;
}

char const* BtcK_Error_Domain(struct BtcK_Error const* error)
{
  return error->domain;
}

char const* BtcK_Error_Message(struct BtcK_Error const* error)
{
  return error->message;
}
