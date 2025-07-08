// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <Python.h>

#include <btck/btck.h>

#include "src/_slice.h"
#include "src/block.h"
#include "src/block_hash.h"
#include "src/chain.h"
#include "src/script_pubkey.h"
#include "src/transaction.h"
#include "src/transaction_output.h"

// PyObject* ComparisonNotImplemented(void const* left, void const* right, int op)
// {
//   static char const* const opstrings[] = {"<", "<=", "==", "!=", ">", ">="};
//   return PyErr_Format(
//     PyExc_TypeError, "'%s' is not supported between instances of %R and %R", opstrings[op], Py_TYPE(left),
//     Py_TYPE(right)
//   );
// }

// static int convert_txout_iter(PyObject* object, kernel_Warning* out)
// {
//   PyObject* sequence = PySequence_Fast(object, "Not a sequence");
//   if (sequence == NULL) {
//     return 0;
//   }
//   return 0;
// }

// PyDoc_STRVAR(verify_script_doc, "TODO: Write docstring");
// static PyObject* verify_script(PyObject* self, PyObject* args, PyObject* kwargs)
// {
//   static char* kwlist[] = {"script_pubkey", "amount", "tx_to", "spent_outputs", "input_index", "flags", NULL};
//
//   PyObject* script_pubkey = NULL;
//   int64_t amount = 0;
//   PyObject* tx_to = NULL;
//   PyObject* spent_outputs = NULL;  // TODO: convert (wth converter) into array
//   unsigned int input_index = 0;
//   kernel_ScriptFlags flags = kernel_SCRIPT_FLAGS_VERIFY_NONE;
//
//   // clang-format off
//   if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!LOIO&", kwlist,
//       &ScriptPubkey_Type, &script_pubkey,
//       &amount,
//       &Transaction_Type, &tx_to,
//       &spent_outputs,
//       &input_index,
//       ScriptFlags_Convert, &flags)) {
//     return NULL;
//   }
//   // clang-format on
//
//   kernel_ScriptVerifyStatus status;
//   bool result = kernel_verify_script(
//     ScriptPubkey_GetImpl(script_pubkey), amount, Transaction_GetImpl(tx_to), NULL, 0, input_index, flags, &status
//   );
//
//   return ScriptVerifyStatus_New(status);
// }

static PyTypeObject* const types[] = {
  &Slice_Type,        &Block_Type,       &BlockHash_Type,         &Chain_Type,
  &ScriptPubkey_Type, &Transaction_Type, &TransactionOutput_Type,
};

static int exec(PyObject* module)
{
  for (int idx = 0; idx < Py_ARRAY_LENGTH(types); ++idx) {
    if (PyModule_AddType(module, types[idx]) < 0) {
      return -1;
    }
  }

  return 0;
}

static PyModuleDef module = {
  .m_base = PyModuleDef_HEAD_INIT,
  .m_name = "btck",
  .m_doc = "btck module documentation",
  // .m_methods =
  //   (PyMethodDef[]){
  //     {
  //       "verify_script",
  //       (PyCFunction)verify_script,
  //       METH_VARARGS | METH_KEYWORDS,
  //       verify_script_doc,
  //     },
  //     {},
  //   },
  .m_slots = (PyModuleDef_Slot[]){
    {Py_mod_exec, exec},
    {},
  },
};

PyMODINIT_FUNC PyInit_btck(void);
PyMODINIT_FUNC PyInit_btck(void)
{
  return PyModuleDef_Init(&module);
}
