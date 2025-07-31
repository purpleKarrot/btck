// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <Python.h>

struct BtcK_TransactionOutput;

extern PyTypeObject TransactionOutput_Type;

PyObject* TransactionOutput_New(struct BtcK_TransactionOutput* txout);
struct BtcK_TransactionOutput* TransactionOutput_GetImpl(PyObject* object);
