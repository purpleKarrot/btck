// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <Python.h>

struct BtcK_Block;

extern PyTypeObject Block_Type;
extern PyTypeObject Block_TransactionsSlice_Type;

PyObject* Block_New(struct BtcK_Block* block);
