// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "verification_flags.h"

#include <assert.h>
#include <stddef.h>

struct Self
{
  PyObject_HEAD
  BtcK_VerificationFlags impl;
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

PyTypeObject VerificationFlags_Type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "btck.VerificationFlags",
  .tp_doc = "VerificationFlags object",
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
  {"ALL", {PyObject_HEAD_INIT(&VerificationFlags_Type) BtcK_VerificationFlags_ALL}},
  {"NONE", {PyObject_HEAD_INIT(&VerificationFlags_Type) BtcK_VerificationFlags_NONE}},
  {"P2SH", {PyObject_HEAD_INIT(&VerificationFlags_Type) BtcK_VerificationFlags_P2SH}},
  {"DERSIG", {PyObject_HEAD_INIT(&VerificationFlags_Type) BtcK_VerificationFlags_DERSIG}},
  {"NULLDUMMY",
   {PyObject_HEAD_INIT(
     &VerificationFlags_Type
   ) BtcK_VerificationFlags_NULLDUMMY}},
  {"CHECKLOCKTIMEVERIFY",
   { PyObject_HEAD_INIT(&VerificationFlags_Type)
    BtcK_VerificationFlags_CHECKLOCKTIMEVERIFY}},
  {"CHECKSEQUENCEVERIFY",
   {PyObject_HEAD_INIT(&VerificationFlags_Type)
    BtcK_VerificationFlags_CHECKSEQUENCEVERIFY}},
  {"WITNESS",
   {PyObject_HEAD_INIT(
     &VerificationFlags_Type
   ) BtcK_VerificationFlags_WITNESS}},
  {"TAPROOT",
   {PyObject_HEAD_INIT(
     &VerificationFlags_Type
   ) BtcK_VerificationFlags_TAPROOT}},
  {},
};

static PyObject* nb_or(struct Self const* left, struct Self const* right)
{
  return VerificationFlags_New(left->impl | right->impl);
}

static PyObject* nb_and(struct Self const* left, struct Self const* right)
{
  return VerificationFlags_New(left->impl & right->impl);
}

static PyObject* nb_xor(struct Self const* left, struct Self const* right)
{
  return VerificationFlags_New(left->impl ^ right->impl);
}

static PyObject* nb_invert(struct Self const* self)
{
  return VerificationFlags_New(~self->impl);
}

static PyObject* ComparisonNotImplemented(
  void const* left, void const* right, int op
)
{
  static char const* const opstrings[] = {"<", "<=", "==", "!=", ">", ">="};
  return PyErr_Format(
    PyExc_TypeError, "'%s' is not supported between instances of %R and %R",
    opstrings[op], Py_TYPE(left), Py_TYPE(right)
  );
}

static PyObject* richcmp(struct Self const* self, PyObject* other, int op)
{
  if ((op != Py_EQ && op != Py_NE) ||
      !PyObject_TypeCheck(other, &VerificationFlags_Type)) {
    return ComparisonNotImplemented(self, other, op);
  }

  return PyBool_FromLong(
    (op == Py_EQ) == (self->impl == ((struct Self*)other)->impl)
  );
}

void VerificationFlags_Init(void)
{
  PyObject* dict = VerificationFlags_Type.tp_dict;
  for (struct Enum* e = enums; e->name != NULL; ++e) {
    PyDict_SetItemString(dict, e->name, (PyObject*)&e->value);
  }
}

PyObject* VerificationFlags_New(BtcK_VerificationFlags value)
{
  struct Self* self = PyObject_New(struct Self, &VerificationFlags_Type);
  if (self == NULL) {
    return NULL;
  }
  self->impl = value;
  return (PyObject*)self;
}

BtcK_VerificationFlags VerificationFlags_GetImpl(PyObject* object)
{
  assert(PyObject_TypeCheck(object, &VerificationFlags_Type));
  return ((struct Self*)object)->impl;
}
