// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <Python.h>

#include <btck/btck.h>

extern PyTypeObject VerificationFlags_Type;

void VerificationFlags_Init(void);
PyObject* VerificationFlags_New(BtcK_VerificationFlags value);
int VerificationFlags_Convert(PyObject* object, BtcK_VerificationFlags* out);
