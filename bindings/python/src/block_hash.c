// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "block_hash.h"

#include <btck/btck.h>

struct Self
{
  PyObject_HEAD
  struct btck_BlockHash impl;
};

static PyObject* new(PyTypeObject* type, PyObject* args, PyObject* kwargs);
static int getbuffer(struct Self const* self, Py_buffer* view, int flags);
static PyObject* bytes(struct Self const* self, PyObject* ignored);

PyTypeObject BlockHash_Type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "btck.BlockHash",
  .tp_doc = "BlockHash object",
  .tp_basicsize = sizeof(struct Self),
  .tp_flags = Py_TPFLAGS_DEFAULT,
  .tp_new = new,
  .tp_as_buffer =
    &(PyBufferProcs){
      .bf_getbuffer = (getbufferproc)getbuffer,
    },
  .tp_methods = (PyMethodDef[]){
    {"__bytes__", (PyCFunction)bytes, METH_NOARGS, ""},
    {},
  },
};

static PyObject* new(PyTypeObject* Py_UNUSED(type), PyObject* args, PyObject* kwargs)
{
  static char* kwlist[] = {"raw", NULL};

  Py_buffer raw;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "y*", kwlist, &raw)) {
    return NULL;
  }

  if (raw.len != btck_BlockHash_SIZE) {
    PyErr_SetString(PyExc_ValueError, "Invalid block hash length");
    return NULL;
  }

  struct btck_BlockHash hash;
  btck_BlockHash_Init(&hash, raw.buf, raw.len);
  return BlockHash_New(&hash);
}

static int getbuffer(struct Self const* self, Py_buffer* view, int flags)
{
  return PyBuffer_FillInfo(view, (PyObject*)self, (void*)self->impl.data, btck_BlockHash_SIZE, true, flags);
}

static PyObject* bytes(struct Self const* self, PyObject* Py_UNUSED(ignored))
{
  return PyBytes_FromStringAndSize((char const*)self->impl.data, btck_BlockHash_SIZE);
}

PyObject* BlockHash_New(struct btck_BlockHash const* hash)
{
  struct Self* self = PyObject_New(struct Self, &BlockHash_Type);
  if (self == NULL) {
    return NULL;
  }

  self->impl = *hash;
  return (PyObject*)self;
}
