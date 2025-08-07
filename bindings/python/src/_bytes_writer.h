// This code is taken from the reference implementation of PEP 782
// https://github.com/python/cpython/pull/131681

#include <Python.h>
#include <stddef.h>

typedef struct PyBytesWriter PyBytesWriter;

PyBytesWriter* PyBytesWriter_Create(Py_ssize_t size);

void PyBytesWriter_Discard(PyBytesWriter* writer);

PyObject* PyBytesWriter_Finish(PyBytesWriter* writer);

int PyBytesWriter_WriteBytes(
  PyBytesWriter* writer, void const* bytes, Py_ssize_t size);

int write_bytes(void const* bytes, size_t size, void* writer);
