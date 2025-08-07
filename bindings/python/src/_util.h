// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <btck/btck.h>

#include <Python.h>

PyObject* cmp_not_implemented(void const* left, void const* right, int op);

typedef int (*to_bytes_fn)(void const*, BtcK_WriteBytes, void*);
PyObject* to_bytes(void const* obj, to_bytes_fn writefn);

typedef int (*to_string_fn)(void const*, char*, size_t);
PyObject* to_string(void const* obj, to_string_fn printfn);
