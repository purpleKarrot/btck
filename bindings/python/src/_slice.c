// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "_slice.h"

#include <stddef.h>

struct Self {
  PyObject_HEAD
  PyObject* obj;
  Py_ssize_t length;
  Py_ssize_t start;
  Py_ssize_t step;
  ssizeargfunc item;
};

static void dealloc(struct Self* self)
{
  Py_DECREF(self->obj);
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static Py_ssize_t length(struct Self const* self)
{
  return self->length;
}

static PyObject* item(struct Self const* self, Py_ssize_t idx)
{
  if (idx < 0 || idx >= self->length) {
    PyErr_SetString(PyExc_IndexError, "index out of range");
    return NULL;
  }

  if (self->item != NULL) {
    return self->item(self->obj, idx);
  }

  PySequenceMethods const* seq = Py_TYPE(self->obj)->tp_as_sequence;
  return seq->sq_item(self->obj, self->start + (idx * self->step));
}

static PyObject* subscript(struct Self const* self, PyObject* arg)
{
  if (PyIndex_Check(arg)) {
    Py_ssize_t idx = PyNumber_AsSsize_t(arg, PyExc_IndexError);
    if (idx == -1 && PyErr_Occurred()) {
      return NULL;
    }
    if (idx < 0) {
      idx += self->length;
    }
    return item(self, idx);
  }

  if (PySlice_Check(arg)) {
    Py_ssize_t start, stop, step;  // NOLINT(readability-isolate-declaration)
    if (PySlice_Unpack(arg, &start, &stop, &step) < 0) {
      return NULL;
    }

    struct Self* slice = PyObject_New(struct Self, &Slice_Type);
    if (slice == NULL) {
      return NULL;
    }

    Py_ssize_t const length =
      PySlice_AdjustIndices(self->length, &start, &stop, step);

    slice->obj = Py_NewRef(self);
    slice->length = length;
    slice->start = start;
    slice->step = step;

    return (PyObject*)slice;
  }

  return PyErr_Format(
    PyExc_TypeError, "indices must be integers or slices, not %.200s",
    Py_TYPE(arg)->tp_name);
}

static PySequenceMethods as_sequence = {
  .sq_length = (lenfunc)length,
  .sq_item = (ssizeargfunc)item,
};

static PyMappingMethods as_mapping = {
  .mp_length = (lenfunc)length,
  .mp_subscript = (binaryfunc)subscript,
};

PyTypeObject Slice_Type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "btck._Slice",
  .tp_basicsize = sizeof(struct Self),
  .tp_dealloc = (destructor)dealloc,
  .tp_as_sequence = &as_sequence,
  .tp_as_mapping = &as_mapping,
  .tp_flags = Py_TPFLAGS_DEFAULT,
};

PyObject* Slice_New(PyObject* obj, Py_ssize_t length, ssizeargfunc item)
{
  struct Self* slice = PyObject_New(struct Self, &Slice_Type);
  if (slice == NULL) {
    return NULL;
  }

  slice->obj = Py_NewRef(obj);
  slice->length = length;
  slice->item = item;

  return (PyObject*)slice;
}
