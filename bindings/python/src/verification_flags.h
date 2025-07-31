// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <btck/btck.h>

#include <Python.h>

extern PyTypeObject VerificationFlags_Type;

void VerificationFlags_Init(void);
PyObject* VerificationFlags_New(BtcK_VerificationFlags value);
BtcK_VerificationFlags VerificationFlags_GetImpl(PyObject* object);
