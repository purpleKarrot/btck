// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "_util.h"

#include "_bytes_writer.h"

static int write_bytes(void const* bytes, size_t size, void* writer)
{
  return PyBytesWriter_WriteBytes(
    (PyBytesWriter*)writer, bytes, (Py_ssize_t)size);
}

PyObject* cmp_not_implemented(void const* left, void const* right, int op)
{
  static char const* const opstrings[] = {"<", "<=", "==", "!=", ">", ">="};
  return PyErr_Format(
    PyExc_TypeError, "'%s' is not supported between instances of %R and %R",
    opstrings[op], Py_TYPE(left), Py_TYPE(right));
}

PyObject* to_bytes(void const* obj, to_bytes_fn writefn)
{
  PyBytesWriter* writer = PyBytesWriter_Create(0);
  if (writefn(obj, write_bytes, writer) != 0) {
    PyBytesWriter_Discard(writer);
    return NULL;
  }

  return PyBytesWriter_Finish(writer);
}

PyObject* to_string(void const* obj, to_string_fn printfn)
{
  int const length = printfn(obj, NULL, 0);
  if (length < 0) {
    PyErr_SetNone(PyExc_ValueError);
    return NULL;
  }

  PyObject* ret = PyUnicode_New(length, 127);
  if (ret == NULL) {
    return NULL;
  }

  (void)printfn(obj, PyUnicode_DATA(ret), length + 1);
  return ret;
}
