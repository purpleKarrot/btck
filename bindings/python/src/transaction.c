// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "transaction.h"

#include <btck/btck.h>

#include <assert.h>
#include <stddef.h>

#include "_error.h"
#include "_slice.h"
#include "transaction_output.h"

struct Self {
  PyObject_HEAD
  struct BtcK_Transaction* impl;
};

static void dealloc(struct Self* self);
static PyObject* new(PyTypeObject* type, PyObject* args, PyObject* kwargs);
static PyObject* get_outputs(struct Self const* self, void* closure);

static PyGetSetDef getset[] = {
  {"outputs", (getter)get_outputs, NULL, "", NULL},
  {},
};

PyTypeObject Transaction_Type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "btck.Transaction",
  .tp_doc = "Transaction object",
  .tp_basicsize = sizeof(struct Self),
  .tp_dealloc = (destructor)dealloc,
  .tp_flags = Py_TPFLAGS_DEFAULT,
  .tp_new = new,
  .tp_getset = getset,
};

static void dealloc(struct Self* self)
{
  BtcK_Transaction_Free(self->impl);
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* new(
  PyTypeObject* Py_UNUSED(type), PyObject* args, PyObject* kwargs)
{
  static char* kwlist[] = {"raw", NULL};

  Py_buffer buffer;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "y*", kwlist, &buffer)) {
    return NULL;
  }

  return Transaction_New(BtcK_Transaction_New(buffer.buf, buffer.len, NULL));
}

static PyObject* outputs_item(struct Self* self, Py_ssize_t idx)
{
  struct BtcK_Error* err = NULL;
  struct BtcK_TransactionOutput* ptr = BtcK_TransactionOutput_Copy(
    BtcK_Transaction_GetOutput(self->impl, idx), &err);
  if (err != NULL) {
    return SetError(err);
  }
  return TransactionOutput_New(ptr);
}

static PyObject* get_outputs(struct Self const* self, void* Py_UNUSED(closure))
{
  Py_ssize_t const length =
    (Py_ssize_t)BtcK_Transaction_CountOutputs(self->impl);
  return Slice_New((PyObject*)self, length, (ssizeargfunc)outputs_item);
}

PyObject* Transaction_New(struct BtcK_Transaction* tx)
{
  struct Self* self = PyObject_New(struct Self, &Transaction_Type);
  if (self == NULL) {
    BtcK_Transaction_Free(tx);
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
