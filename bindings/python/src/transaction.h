// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <Python.h>

struct BtcK_Transaction;

extern PyTypeObject Transaction_Type;
extern PyTypeObject Transaction_OutputsSlice_Type;

PyObject* Transaction_New(struct BtcK_Transaction* tx);
struct BtcK_Transaction* Transaction_GetImpl(PyObject* object);
