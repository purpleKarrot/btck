// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "block.h"

#include <btck/btck.h>

#include <stddef.h>

#include "_error.h"
#include "_slice.h"
#include "block_hash.h"
#include "transaction.h"

struct Self {
  PyObject_HEAD
  struct BtcK_Block* impl;
};

static void dealloc(struct Self* self);
static PyObject* new(PyTypeObject* type, PyObject* args, PyObject* kwargs);
static PyObject* get_hash(struct Self const* self, void* closure);
static PyObject* get_transactions(struct Self const* self, void* closure);

static PyGetSetDef getset[] = {
  {"hash", (getter)get_hash, NULL, "", NULL},
  {"transactions", (getter)get_transactions, NULL, "", NULL},
  {},
};

PyTypeObject Block_Type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "btck.Block",
  .tp_doc = "Block object",
  .tp_basicsize = sizeof(struct Self),
  .tp_dealloc = (destructor)dealloc,
  .tp_flags = Py_TPFLAGS_DEFAULT,
  .tp_new = new,
  .tp_getset = getset,
};

static void dealloc(struct Self* self)
{
  BtcK_Block_Free(self->impl);
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* new(
  PyTypeObject* Py_UNUSED(type), PyObject* args, PyObject* kwargs)
{
  static char* kwlist[] = {"", NULL};

  Py_buffer raw;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "y*", kwlist, &raw)) {
    return NULL;
  }

  return Block_New(BtcK_Block_New(raw.buf, raw.len, NULL));
}

static PyObject* get_hash(struct Self const* self, void* Py_UNUSED(closure))
{
  struct BtcK_BlockHash hash;
  BtcK_Block_GetHash(self->impl, &hash);
  return BlockHash_New(&hash);
}

static PyObject* transactions_item(struct Self* self, Py_ssize_t idx)
{
  struct BtcK_Error* err = NULL;
  struct BtcK_Transaction* ptr =
    BtcK_Block_GetTransaction(self->impl, idx, &err);
  if (err != NULL) {
    return SetError(err);
  }
  return Transaction_New(ptr);
}

static PyObject* get_transactions(
  struct Self const* self, void* Py_UNUSED(closure))
{
  Py_ssize_t const length =
    (Py_ssize_t)BtcK_Block_CountTransactions(self->impl);
  return Slice_New((PyObject*)self, length, (ssizeargfunc)transactions_item);
}

PyObject* Block_New(struct BtcK_Block* block)
{
  struct Self* self = PyObject_New(struct Self, &Block_Type);
  if (self == NULL) {
    BtcK_Block_Free(block);
    return NULL;
  }
  self->impl = block;
  return (PyObject*)self;
}
