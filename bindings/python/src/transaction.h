// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <Python.h>
#include <stdbool.h>

#include <btck/btck.h>

extern PyTypeObject Transaction_Type;

PyObject* Transaction_New(struct btck_Transaction* tx);
struct btck_Transaction* Transaction_GetImpl(PyObject* object);
