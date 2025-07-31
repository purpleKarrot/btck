// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "_error.h"

#include <btck/btck.h>

#include <stddef.h>
#include <string.h>

PyObject* VerificationError = NULL;

PyObject* SetError(struct BtcK_Error* error)
{
  int const code = BtcK_Error_Code(error);
  char const* const domain = BtcK_Error_Domain(error);
  char const* const message = BtcK_Error_Message(error);

  if (strcmp(domain, "Memory") == 0) {
    PyErr_SetNone(PyExc_MemoryError);
  }
  else if (strcmp(domain, "VerificationError") == 0) {
    PyErr_SetString(VerificationError, message);
  }
  else {
    PyErr_SetString(PyExc_RuntimeError, message);
  }

  BtcK_Error_Free(error);
  return NULL;
}
