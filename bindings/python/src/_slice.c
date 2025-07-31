// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "_slice.h"

#include <stddef.h>

struct Self {
  PyObject_HEAD
  PyObject* seq;
  Py_ssize_t start;
  Py_ssize_t step;
  Py_ssize_t length;
};

static void dealloc(struct Self* self)
{
  Py_DECREF(self->seq);
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

  PySequenceMethods const* seq = Py_TYPE(self->seq)->tp_as_sequence;
  return seq->sq_item(self->seq, self->start + (idx * self->step));
}

static PySequenceMethods as_sequence = {
  .sq_length = (lenfunc)length,
  .sq_item = (ssizeargfunc)item,
};

static PyMappingMethods as_mapping = {
  .mp_length = (lenfunc)length,
  .mp_subscript = Slice_subscript,
};

PyTypeObject Slice_Type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "btck._Slice",
  .tp_basicsize = sizeof(struct Self),
  .tp_dealloc = (destructor)dealloc,
  .tp_as_sequence = &as_sequence,
  .tp_as_mapping = &as_mapping,
  .tp_flags = Py_TPFLAGS_DEFAULT,
  .tp_doc = "Lazy slice view of a sequence",
};

PyObject* Slice_subscript(PyObject* self, PyObject* arg)
{
  PySequenceMethods const* seq = Py_TYPE(self)->tp_as_sequence;

  if (PyIndex_Check(arg)) {
    Py_ssize_t idx = PyNumber_AsSsize_t(arg, PyExc_IndexError);
    if (idx == -1 && PyErr_Occurred()) {
      return NULL;
    }
    if (idx < 0) {
      idx += seq->sq_length(self);
    }
    return seq->sq_item(self, idx);
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
      PySlice_AdjustIndices(seq->sq_length(self), &start, &stop, step);

    slice->seq = Py_NewRef(self);
    slice->start = start;
    slice->step = step;
    slice->length = length;

    return (PyObject*)slice;
  }

  return PyErr_Format(
    PyExc_TypeError, "indices must be integers or slices, not %.200s",
    Py_TYPE(arg)->tp_name);
}
