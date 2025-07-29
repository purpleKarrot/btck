// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "script_verify.h"

#include <assert.h>
#include <stddef.h>

struct Self
{
  PyObject_HEAD
  BtcK_ScriptVerify impl;
};

static PyObject* new(PyTypeObject* type, PyObject* args, PyObject* kwargs);
static PyObject* richcmp(struct Self const* self, PyObject* other, int op);
static PyObject* nb_or(struct Self const* left, struct Self const* right);
static PyObject* nb_and(struct Self const* left, struct Self const* right);
static PyObject* nb_xor(struct Self const* left, struct Self const* right);
static PyObject* nb_invert(struct Self const* self);

static PyNumberMethods as_number = {
  .nb_or = (binaryfunc)nb_or,
  .nb_and = (binaryfunc)nb_and,
  .nb_xor = (binaryfunc)nb_xor,
  .nb_invert = (unaryfunc)nb_invert,
};

PyTypeObject ScriptVerify_Type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "btck.ScriptVerify",
  .tp_doc = "ScriptVerify object",
  .tp_basicsize = sizeof(struct Self),
  .tp_flags = Py_TPFLAGS_DEFAULT,
  .tp_richcompare = (richcmpfunc)richcmp,
  .tp_as_number = &as_number,
};

struct Enum
{
  char const* name;
  struct Self value;
};

static struct Enum enums[] = {
  {"ALL", {PyObject_HEAD_INIT(&ScriptVerify_Type) BtcK_ScriptVerify_ALL}},
  {"NONE", {PyObject_HEAD_INIT(&ScriptVerify_Type) BtcK_ScriptVerify_NONE}},
  {"P2SH", {PyObject_HEAD_INIT(&ScriptVerify_Type) BtcK_ScriptVerify_P2SH}},
  {"DERSIG", {PyObject_HEAD_INIT(&ScriptVerify_Type) BtcK_ScriptVerify_DERSIG}},
  {"NULLDUMMY", {PyObject_HEAD_INIT(&ScriptVerify_Type) BtcK_ScriptVerify_NULLDUMMY}},
  {"CHECKLOCKTIMEVERIFY",
   { PyObject_HEAD_INIT(
     &ScriptVerify_Type
   ) BtcK_ScriptVerify_CHECKLOCKTIMEVERIFY}},
  {"CHECKSEQUENCEVERIFY",
   {PyObject_HEAD_INIT(
     &ScriptVerify_Type
   ) BtcK_ScriptVerify_CHECKSEQUENCEVERIFY}},
  {"WITNESS", {PyObject_HEAD_INIT(&ScriptVerify_Type) BtcK_ScriptVerify_WITNESS}},
  {"TAPROOT", {PyObject_HEAD_INIT(&ScriptVerify_Type) BtcK_ScriptVerify_TAPROOT}},
  {},
};

static PyObject* nb_or(struct Self const* left, struct Self const* right)
{
  return ScriptVerify_New(left->impl | right->impl);
}

static PyObject* nb_and(struct Self const* left, struct Self const* right)
{
  return ScriptVerify_New(left->impl & right->impl);
}

static PyObject* nb_xor(struct Self const* left, struct Self const* right)
{
  return ScriptVerify_New(left->impl ^ right->impl);
}

static PyObject* nb_invert(struct Self const* self)
{
  return ScriptVerify_New(~self->impl);
}

static PyObject* ComparisonNotImplemented(void const* left, void const* right, int op)
{
  static char const* const opstrings[] = {"<", "<=", "==", "!=", ">", ">="};
  return PyErr_Format(
    PyExc_TypeError, "'%s' is not supported between instances of %R and %R", opstrings[op], Py_TYPE(left),
    Py_TYPE(right)
  );
}

static PyObject* richcmp(struct Self const* self, PyObject* other, int op)
{
  if ((op != Py_EQ && op != Py_NE) ||
      !PyObject_TypeCheck(other, &ScriptVerify_Type)) {
    return ComparisonNotImplemented(self, other, op);
  }

  return PyBool_FromLong(
    (op == Py_EQ) == (self->impl == ((struct Self*)other)->impl)
  );
}

void ScriptVerify_Init(void)
{
  PyObject* dict = ScriptVerify_Type.tp_dict;
  for (struct Enum* e = enums; e->name != NULL; ++e) {
    PyDict_SetItemString(dict, e->name, (PyObject*)&e->value);
  }
}

PyObject* ScriptVerify_New(BtcK_ScriptVerify value)
{
  struct Self* self = PyObject_New(struct Self, &ScriptVerify_Type);
  if (self == NULL) {
    return NULL;
  }
  self->impl = value;
  return (PyObject*)self;
}

BtcK_ScriptVerify ScriptVerify_GetImpl(PyObject* object)
{
  assert(PyObject_TypeCheck(object, &ScriptVerify_Type));
  return ((struct Self*)object)->impl;
}
