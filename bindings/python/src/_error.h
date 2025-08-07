// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <Python.h>

struct BtcK_Error;

extern PyObject* VerificationError;
PyObject* SetError(struct BtcK_Error* error);
