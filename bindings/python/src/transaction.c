// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "transaction.h"

#include <assert.h>
#include <stddef.h>

#include <btck/btck.h>

#include "_slice.h"
#include "transaction_output.h"

struct BtcK_Transaction;

struct Self
{
  PyObject_HEAD
  struct BtcK_Transaction* impl;
};

static void dealloc(struct Self* self);
static PyObject* new(PyTypeObject* type, PyObject* args, PyObject* kwargs);
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

PyTypeObject Transaction_Type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "btck.Transaction",
  .tp_doc = "Transaction object",
  .tp_basicsize = sizeof(struct Self),
  .tp_dealloc = (destructor)dealloc,
  .tp_flags = Py_TPFLAGS_DEFAULT,
  .tp_new = new,
  .tp_as_sequence = &as_sequence,
  .tp_as_mapping = &as_mapping,
};

static void dealloc(struct Self* self)
{
  BtcK_Transaction_Release(self->impl);
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* new(
  PyTypeObject* Py_UNUSED(type), PyObject* args, PyObject* kwargs
)
{
  static char* kwlist[] = {"raw", NULL};

  Py_buffer buffer;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "y*", kwlist, &buffer)) {
    return NULL;
  }

  return Transaction_New(BtcK_Transaction_New(buffer.buf, buffer.len, NULL));
}

static Py_ssize_t length(struct Self const* self)
{
  return (Py_ssize_t)BtcK_Transaction_GetSize(self->impl);
}

static PyObject* item(struct Self* self, Py_ssize_t idx)
{
  if (idx < 0 || idx >= length(self)) {
    PyErr_SetString(PyExc_IndexError, "index out of range");
    return NULL;
  }
  return TransactionOutput_New(BtcK_Transaction_At(self->impl, idx));
}

PyObject* Transaction_New(struct BtcK_Transaction* tx)
{
  struct Self* self = PyObject_New(struct Self, &Transaction_Type);
  if (self == NULL) {
    BtcK_Transaction_Release(tx);
    return NULL;
  }
  self->impl = tx;
  return (PyObject*)self;
}

struct BtcK_Transaction* Transaction_GetImpl(PyObject* object)
{
  assert(PyObject_TypeCheck(object, &Transaction_Type));
  return ((struct Self*)object)->impl;
}
