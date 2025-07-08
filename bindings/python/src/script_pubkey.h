// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <Python.h>
#include <stdbool.h>

#include <btck/btck.h>

extern PyTypeObject ScriptPubkey_Type;

PyObject* ScriptPubkey_New(struct btck_ScriptPubkey* script_pubkey);
struct btck_ScriptPubkey* ScriptPubkey_GetImpl(PyObject* object);
