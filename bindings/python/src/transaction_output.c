// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "transaction_output.h"

#include <btck/btck.h>

#include <stddef.h>

#include "script_pubkey.h"

struct BtcK_TransactionOutput;

struct Self {
  PyObject_HEAD
  struct BtcK_TransactionOutput* impl;
};

static void dealloc(struct Self* self);
static PyObject* new(PyTypeObject* type, PyObject* args, PyObject* kwargs);
static PyObject* get_amount(struct Self const* self, void* closure);
static PyObject* get_script_pubkey(struct Self const* self, void* closure);

static PyGetSetDef getset[] = {
  {"amount", (getter)get_amount, NULL, "", NULL},
  {"script_pubkey", (getter)get_script_pubkey, NULL, "", NULL},
  {},
};

PyTypeObject TransactionOutput_Type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "btck.TransactionOutput",
  .tp_doc = "TransactionOutput object",
  .tp_basicsize = sizeof(struct Self),
  .tp_dealloc = (destructor)dealloc,
  .tp_flags = Py_TPFLAGS_DEFAULT,
  .tp_new = new,
  .tp_getset = getset,
};

static void dealloc(struct Self* self)
{
  BtcK_TransactionOutput_Release(self->impl);
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* new(
  PyTypeObject* Py_UNUSED(type), PyObject* args, PyObject* kwargs)
{
  static char* kwlist[] = {"amount", "script_pubkey", NULL};

  long long amount;
  PyObject* script_pubkey = NULL;
  if (!PyArg_ParseTupleAndKeywords(
        args, kwargs, "LO!", kwlist, &amount, ScriptPubkey_Type,
        &script_pubkey)) {
    return NULL;
  }

  return TransactionOutput_New(BtcK_TransactionOutput_New(
    amount, ScriptPubkey_GetImpl(script_pubkey), NULL));
}

static PyObject* get_amount(struct Self const* self, void* Py_UNUSED(closure))
{
  return PyLong_FromLongLong(BtcK_TransactionOutput_GetAmount(self->impl));
}

static PyObject* get_script_pubkey(
  struct Self const* self, void* Py_UNUSED(closure))
{
  return ScriptPubkey_New(
    BtcK_TransactionOutput_GetScriptPubkey(self->impl, NULL));
}

PyObject* TransactionOutput_New(struct BtcK_TransactionOutput* txout)
{
  struct Self* self = PyObject_New(struct Self, &TransactionOutput_Type);
  if (self == NULL) {
    BtcK_TransactionOutput_Release(txout);
    return NULL;
  }
  self->impl = txout;
  return (PyObject*)self;
}
