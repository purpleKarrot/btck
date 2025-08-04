// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "block.h"

#include <btck/btck.h>

#include <stddef.h>

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
static PyObject* get_transactions_slice(struct Self const* self, void* closure);

static Py_ssize_t num_transactions(struct Self const* self);
static PyObject* get_transaction(struct Self* self, Py_ssize_t idx);

static PySequenceMethods transactions_as_sequence = {
  .sq_length = (lenfunc)num_transactions,
  .sq_item = (ssizeargfunc)get_transaction,
};

static PyMappingMethods transactions_as_mapping = {
  .mp_length = (lenfunc)num_transactions,
  .mp_subscript = Slice_subscript,
};

static PyGetSetDef getset[] = {
  {"hash", (getter)get_hash, NULL, "", NULL},
  {"transactions", (getter)get_transactions_slice, NULL, "", NULL},
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

PyTypeObject Block_TransactionsSlice_Type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "btck._Slice[Transaction]",
  .tp_basicsize = sizeof(struct Self),
  .tp_dealloc = (destructor)dealloc,
  .tp_flags = Py_TPFLAGS_DEFAULT,
  .tp_as_sequence = &transactions_as_sequence,
  .tp_as_mapping = &transactions_as_mapping,
};

static void dealloc(struct Self* self)
{
  BtcK_Block_Release(self->impl);
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

static PyObject* get_transactions_slice(
  struct Self const* self, void* Py_UNUSED(closure))
{
  struct Self* slice = PyObject_New(struct Self, &Block_TransactionsSlice_Type);
  if (slice == NULL) {
    return NULL;
  }
  slice->impl = BtcK_Block_Retain(self->impl);
  return (PyObject*)slice;
}

static Py_ssize_t num_transactions(struct Self const* self)
{
  return (Py_ssize_t)BtcK_Block_CountTransactions(self->impl);
}

static PyObject* get_transaction(struct Self* self, Py_ssize_t idx)
{
  if (idx < 0 || idx >= num_transactions(self)) {
    PyErr_SetString(PyExc_IndexError, "index out of range");
    return NULL;
  }
  return Transaction_New(BtcK_Block_GetTransaction(self->impl, idx));
}

PyObject* Block_New(struct BtcK_Block* block)
{
  struct Self* self = PyObject_New(struct Self, &Block_Type);
  if (self == NULL) {
    BtcK_Block_Release(block);
    return NULL;
  }
  self->impl = block;
  return (PyObject*)self;
}
