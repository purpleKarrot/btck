// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <btck/btck.h>

#include <Python.h>

#include "src/_error.h"
#include "src/_slice.h"
#include "src/block.h"
#include "src/block_hash.h"
#include "src/chain.h"
#include "src/script_pubkey.h"
#include "src/transaction.h"
#include "src/transaction_output.h"
#include "src/verification_flags.h"

static PyTypeObject* const types[] = {
  &Slice_Type,
  &Block_Type,
  &BlockHash_Type,
  &Chain_Type,
  &ScriptPubkey_Type,
  &Transaction_Type,
  &TransactionOutput_Type,
  &VerificationFlags_Type,
};

static int exec(PyObject* module)
{
  VerificationError =
    PyErr_NewException("btck.VerificationError", PyExc_ValueError, NULL);
  if (VerificationError == NULL) {
    return -1;
  }
  if (PyModule_AddObject(module, "VerificationError", VerificationError) < 0) {
    Py_DECREF(VerificationError);
    return -1;
  }

  for (int idx = 0; idx < Py_ARRAY_LENGTH(types); ++idx) {
    if (PyModule_AddType(module, types[idx]) < 0) {
      return -1;
    }
  }

  VerificationFlags_Init();

  return 0;
}

static PyModuleDef_Slot slots[] = {
  {Py_mod_exec, exec},
  {},
};

static PyModuleDef module = {
  .m_base = PyModuleDef_HEAD_INIT,
  .m_name = "btck",
  .m_doc = "btck module documentation",
  .m_slots = slots,
};

PyMODINIT_FUNC PyInit_btck(void);
PyMODINIT_FUNC PyInit_btck(void)
{
  return PyModuleDef_Init(&module);
}
