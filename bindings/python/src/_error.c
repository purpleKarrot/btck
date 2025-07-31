// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "_error.h"

#include <btck/btck.h>

#include <stddef.h>

PyObject* SetError(struct BtcK_Error* error)
{
  // PyErr_SetNone(PyExc_MemoryError);
  PyErr_SetString(PyExc_RuntimeError, BtcK_Error_Message(error));
  BtcK_Error_Free(error);
  return NULL;
}
