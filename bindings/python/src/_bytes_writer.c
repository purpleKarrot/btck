// This code is taken from the reference implementation of PEP 782
// https://github.com/python/cpython/pull/131681

#include "_bytes_writer.h"

#include <assert.h>
#include <string.h>

struct PyBytesWriter {
  char small_buffer[256];
  PyObject* obj;
  Py_ssize_t size;
};

static inline char* byteswriter_data(PyBytesWriter* writer)
{
  if (writer->obj == NULL) {
    return writer->small_buffer;
  }
  return PyBytes_AS_STRING(writer->obj);
}

static inline Py_ssize_t byteswriter_allocated(PyBytesWriter* writer)
{
  if (writer->obj == NULL) {
    return sizeof(writer->small_buffer);
  }
  return PyBytes_GET_SIZE(writer->obj);
}

#ifdef MS_WINDOWS
/* On Windows, overallocate by 50% is the best factor */
#  define OVERALLOCATE_FACTOR 2
#else
/* On Linux, overallocate by 25% is the best factor */
#  define OVERALLOCATE_FACTOR 4
#endif

static inline int byteswriter_resize(
  PyBytesWriter* writer, Py_ssize_t size, int overallocate)
{
  assert(size >= 0);

  if (size <= byteswriter_allocated(writer)) {
    return 0;
  }

  if (overallocate) {
    if (size <= (PY_SSIZE_T_MAX - size / OVERALLOCATE_FACTOR)) {
      size += size / OVERALLOCATE_FACTOR;
    }
  }

  if (writer->obj != NULL) {
    if (_PyBytes_Resize(&writer->obj, size)) {
      return -1;
    }
    assert(writer->obj != NULL);
  }
  else {
    writer->obj = PyBytes_FromStringAndSize(NULL, size);
    if (writer->obj == NULL) {
      return -1;
    }
    assert((size_t)size > sizeof(writer->small_buffer));
    memcpy(
      PyBytes_AS_STRING(writer->obj), writer->small_buffer,
      sizeof(writer->small_buffer));
  }
  return 0;
}

static int byteswriter_grow(PyBytesWriter* writer, Py_ssize_t size)
{
  if (size < 0 && writer->size + size < 0) {
    PyErr_SetString(PyExc_ValueError, "invalid size");
    return -1;
  }
  if (size > PY_SSIZE_T_MAX - writer->size) {
    PyErr_NoMemory();
    return -1;
  }
  size = writer->size + size;

  if (byteswriter_resize(writer, size, 1) < 0) {
    return -1;
  }
  writer->size = size;
  return 0;
}

PyBytesWriter* PyBytesWriter_Create(Py_ssize_t size)
{
  if (size < 0) {
    PyErr_SetString(PyExc_ValueError, "size must be >= 0");
    return NULL;
  }

  PyBytesWriter* writer = PyMem_Malloc(sizeof(PyBytesWriter));
  if (writer == NULL) {
    PyErr_NoMemory();
    return NULL;
  }

  writer->obj = NULL;
  writer->size = 0;

  if (size >= 1) {
    if (byteswriter_resize(writer, size, 0) < 0) {
      PyBytesWriter_Discard(writer);
      return NULL;
    }
    writer->size = size;
  }
  return writer;
}

void PyBytesWriter_Discard(PyBytesWriter* writer)
{
  if (writer == NULL) {
    return;
  }

  Py_XDECREF(writer->obj);
  PyMem_Free(writer);
}

PyObject* PyBytesWriter_Finish(PyBytesWriter* writer)
{
  PyObject* result;
  if (writer->size == 0) {
    result = PyBytes_FromStringAndSize(NULL, 0);
  }
  else if (writer->obj != NULL) {
    if (writer->size != PyBytes_GET_SIZE(writer->obj)) {
      if (_PyBytes_Resize(&writer->obj, writer->size)) {
        goto error;
      }
    }
    result = writer->obj;
    writer->obj = NULL;
  }
  else {
    result = PyBytes_FromStringAndSize(writer->small_buffer, writer->size);
  }
  PyBytesWriter_Discard(writer);
  return result;

error:
  PyBytesWriter_Discard(writer);
  return NULL;
}

int PyBytesWriter_WriteBytes(
  PyBytesWriter* writer, void const* bytes, Py_ssize_t size)
{
  if (size < 0) {
    size_t len = strlen(bytes);
    if (len > (size_t)PY_SSIZE_T_MAX) {
      PyErr_NoMemory();
      return -1;
    }
    size = (Py_ssize_t)len;
  }

  Py_ssize_t pos = writer->size;
  if (byteswriter_grow(writer, size) < 0) {
    return -1;
  }
  char* buf = byteswriter_data(writer);
  memcpy(buf + pos, bytes, size);
  return 0;
}

int write_bytes(void const* bytes, size_t size, void* writer)
{
  return PyBytesWriter_WriteBytes(
    (PyBytesWriter*)writer, bytes, (Py_ssize_t)size);
}
