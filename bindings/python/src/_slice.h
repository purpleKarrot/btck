// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <Python.h>

extern PyTypeObject Slice_Type;

PyObject* Slice_subscript(PyObject* self, PyObject* arg);
