// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <Python.h>
#include <stdbool.h>

#include <btck/btck.h>

extern PyTypeObject Block_Type;

PyObject* Block_New(struct btck_Block* block);
