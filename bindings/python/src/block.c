// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "block.h"

#include <stddef.h>

#include <btck/btck.h>

#include "_slice.h"
#include "block_hash.h"
#include "transaction.h"

struct BtcK_Block;

struct Self
{
  PyObject_HEAD
  struct BtcK_Block* impl;
};

static void dealloc(struct Self* self);
static PyObject* new(PyTypeObject* type, PyObject* args, PyObject* kwargs);
static PyObject* get_hash(struct Self const* self, void* closure);
static Py_ssize_t length(struct Self const* self);
static PyObject* item(struct Self* self, Py_ssize_t idx);

static PySequenceMethods as_sequence = {
  .sq_length = (lenfunc)length,
  .sq_item = (ssizeargfunc)item,
};

static PyMappingMethods as_mapping = {
  .mp_length = (lenfunc)length,
  .mp_subscript = Slice_subscript,
};

PyTypeObject Block_Type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "btck.Block",
  .tp_doc = "Block object",
  .tp_basicsize = sizeof(struct Self),
  .tp_dealloc = (destructor)dealloc,
  .tp_flags = Py_TPFLAGS_DEFAULT,
  .tp_new = new,
  .tp_as_sequence = &as_sequence,
  .tp_as_mapping = &as_mapping,
  .tp_getset = (PyGetSetDef[]){
    {"hash", (getter)get_hash, NULL, "", NULL},
    {},
  },
};

static void dealloc(struct Self* self)
{
  BtcK_Block_Release(self->impl);
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* new(
  PyTypeObject* Py_UNUSED(type), PyObject* args, PyObject* kwargs
)
{
  static char* kwlist[] = {"", NULL};

  Py_buffer raw;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "y*", kwlist, &raw)) {
    return NULL;
  }

  return Block_New(BtcK_Block_New(raw.buf, raw.len));
}

static PyObject* get_hash(struct Self const* self, void* Py_UNUSED(closure))
{
  struct BtcK_BlockHash hash;
  BtcK_Block_GetHash(self->impl, &hash);
  return BlockHash_New(&hash);
}

static Py_ssize_t length(struct Self const* self)
{
  return (Py_ssize_t)BtcK_Block_GetSize(self->impl);
}

static PyObject* item(struct Self* self, Py_ssize_t idx)
{
  if (idx < 0 || idx >= length(self)) {
    PyErr_SetString(PyExc_IndexError, "index out of range");
    return NULL;
  }
  return Transaction_New(BtcK_Block_At(self->impl, idx));
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
