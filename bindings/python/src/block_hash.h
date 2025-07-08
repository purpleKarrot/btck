// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <Python.h>

struct BtcK_BlockHash;

extern PyTypeObject BlockHash_Type;

PyObject* BlockHash_New(struct BtcK_BlockHash const* hash);
