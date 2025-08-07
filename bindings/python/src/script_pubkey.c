// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "script_pubkey.h"

#include <btck/btck.h>

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "_bytes_writer.h"
#include "_error.h"
#include "transaction.h"
#include "transaction_output.h"
#include "verification_flags.h"

struct Self {
  PyObject_HEAD
  struct BtcK_ScriptPubkey* impl;
};

static void dealloc(struct Self* self);
static PyObject* new(PyTypeObject* type, PyObject* args, PyObject* kwargs);
static PyObject* richcmp(struct Self const* self, PyObject* other, int op);
static PyObject* bytes(struct Self const* self, PyObject* ignored);
static PyObject* verify(
  struct Self const* self, PyObject* args, PyObject* kwargs);

static PyMethodDef methods[] = {
  {"__bytes__", (PyCFunction)bytes, METH_NOARGS, ""},
  {"verify", (PyCFunction)verify, METH_VARARGS | METH_KEYWORDS, ""},
  {},
};

PyTypeObject ScriptPubkey_Type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "btck.ScriptPubkey",
  .tp_doc = "ScriptPubkey object",
  .tp_basicsize = sizeof(struct Self),
  .tp_dealloc = (destructor)dealloc,
  .tp_flags = Py_TPFLAGS_DEFAULT,
  .tp_new = new,
  .tp_richcompare = (richcmpfunc)richcmp,
  .tp_methods = methods,
};

static void dealloc(struct Self* self)
{
  BtcK_ScriptPubkey_Free(self->impl);
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* new(PyTypeObject* type, PyObject* args, PyObject* kwargs)
{
  static char* kwlist[] = {"raw", NULL};

  Py_buffer buffer;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "y*", kwlist, &buffer)) {
    return NULL;
  }

  struct BtcK_Error* error = NULL;
  struct BtcK_ScriptPubkey* impl =
    BtcK_ScriptPubkey_New(buffer.buf, buffer.len, &error);
  if (error != NULL) {
    return SetError(error);
  }

  return ScriptPubkey_New(impl);
}

static PyObject* richcmp(struct Self const* self, PyObject* other, int op)
{
  // if ((op != Py_EQ && op != Py_NE) || !PyObject_TypeCheck(other, ChainType_Type)) {
  //   return ComparisonNotImplemented(self, other, op);
  // }

  // return PyBool_FromLong((op == Py_EQ) == (self->value == ((struct Self*)other)->value));
  return Py_False;
}

static PyObject* bytes(struct Self const* self, PyObject* Py_UNUSED(ignored))
{
  PyBytesWriter* writer = PyBytesWriter_Create(0);
  if (BtcK_ScriptPubkey_ToBytes(self->impl, write_bytes, writer) != 0) {
    PyBytesWriter_Discard(writer);
    return NULL;
  }

  return PyBytesWriter_Finish(writer);
}

struct TxOutputArray {
  struct BtcK_TransactionOutput const** data;
  size_t size;
};

static int convert_tx_outputs(PyObject* obj, struct TxOutputArray* out)
{
  if (Py_IsNone(obj)) {
    out->data = NULL;
    out->size = 0;
    return 1;
  }

  PyObject* seq = PySequence_Fast(obj, "spent_outputs must be a sequence");
  if (seq == NULL) {
    return 0;
  }

  Py_ssize_t const size = PySequence_Fast_GET_SIZE(seq);
  struct BtcK_TransactionOutput const** data =
    PyMem_New(struct BtcK_TransactionOutput const*, size);
  if (data == NULL) {
    Py_DECREF(seq);
    PyErr_NoMemory();
    return 0;
  }

  for (Py_ssize_t idx = 0; idx < size; ++idx) {
    PyObject* item = PySequence_Fast_GET_ITEM(seq, idx);
    if (!PyObject_TypeCheck(item, &TransactionOutput_Type)) {
      PyErr_Format(
        PyExc_TypeError, "spent_outputs[%zd] is not a TransactionOutput", idx);
      PyMem_Free((void*)data);
      Py_DECREF(seq);
      return 0;
    }
    data[idx] = TransactionOutput_GetImpl(item);
  }

  out->data = data;
  out->size = (size_t)size;
  Py_DECREF(seq);
  return 1;
}

static PyObject* verify(
  struct Self const* self, PyObject* args, PyObject* kwargs)
{
  static char* kwlist[] = {
    "amount", "tx_to", "spent_outputs", "input_index", "flags", NULL,
  };

  int64_t amount = 0;
  PyObject* tx_to = NULL;
  struct TxOutputArray spent_outputs = {};
  unsigned int input_index = 0;
  PyObject* flags = NULL;

  if (!PyArg_ParseTupleAndKeywords(
        args, kwargs, "LO!|O&IO!", kwlist, &amount, &Transaction_Type, &tx_to,
        convert_tx_outputs, &spent_outputs, &input_index,
        &VerificationFlags_Type, &flags)) {
    return NULL;
  }

  struct BtcK_Error* error = NULL;
  int const result = BtcK_ScriptPubkey_Verify(
    self->impl, amount, Transaction_GetImpl(tx_to), spent_outputs.data,
    spent_outputs.size, input_index, VerificationFlags_GetImpl(flags), &error);
  if (error != NULL) {
    return SetError(error);
  }

  return PyBool_FromLong(result);
}

PyObject* ScriptPubkey_New(struct BtcK_ScriptPubkey* script_pubkey)
{
  struct Self* self = PyObject_New(struct Self, &ScriptPubkey_Type);
  if (self == NULL) {
    BtcK_ScriptPubkey_Free(script_pubkey);
    return NULL;
  }
  self->impl = script_pubkey;
  return (PyObject*)self;
}

struct BtcK_ScriptPubkey* ScriptPubkey_GetImpl(PyObject* object)
{
  assert(PyObject_TypeCheck(object, &ScriptPubkey_Type));
  return ((struct Self*)object)->impl;
}
