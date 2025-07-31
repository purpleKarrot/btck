// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "script_pubkey.h"

#include <btck/btck.h>

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#include "_error.h"

struct BtcK_ScriptPubkey;

struct Self {
  PyObject_HEAD
  struct BtcK_ScriptPubkey* impl;
};

static void dealloc(struct Self* self);
static PyObject* new(PyTypeObject* type, PyObject* args, PyObject* kwargs);
static PyObject* richcmp(struct Self const* self, PyObject* other, int op);
static int getbuffer(struct Self const* self, Py_buffer* view, int flags);
static PyObject* bytes(struct Self const* self, PyObject* ignored);

static PyBufferProcs as_buffer = {
  .bf_getbuffer = (getbufferproc)getbuffer,
};

static PyMethodDef methods[] = {
  {"__bytes__", (PyCFunction)bytes, METH_NOARGS, ""},
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
  .tp_as_buffer = &as_buffer,
  .tp_methods = methods,
};

static void dealloc(struct Self* self)
{
  BtcK_ScriptPubkey_Release(self->impl);
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

static int getbuffer(struct Self const* self, Py_buffer* view, int flags)
{
  size_t len = 0;
  void const* data = BtcK_ScriptPubkey_AsBytes(self->impl, &len);
  return PyBuffer_FillInfo(
    view, (PyObject*)self, (void*)data, (Py_ssize_t)len, true, flags);
}

static PyObject* bytes(struct Self const* self, PyObject* Py_UNUSED(ignored))
{
  size_t len = 0;
  void const* data = BtcK_ScriptPubkey_AsBytes(self->impl, &len);
  return PyBytes_FromStringAndSize((char const*)data, (Py_ssize_t)len);
}

PyObject* ScriptPubkey_New(struct BtcK_ScriptPubkey* script_pubkey)
{
  struct Self* self = PyObject_New(struct Self, &ScriptPubkey_Type);
  if (self == NULL) {
    BtcK_ScriptPubkey_Release(script_pubkey);
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
