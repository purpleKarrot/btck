// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <Python.h>

#include <btck/btck.h>

extern PyTypeObject ScriptVerify_Type;

void ScriptVerify_Init(void);
PyObject* ScriptVerify_New(BtcK_ScriptVerify value);
int ScriptVerify_Convert(PyObject* object, BtcK_ScriptVerify* out);
