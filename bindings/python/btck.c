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

// PyObject* ComparisonNotImplemented(void const* left, void const* right, int op)
// {
//   static char const* const opstrings[] = {"<", "<=", "==", "!=", ">", ">="};
//   return PyErr_Format(
//     PyExc_TypeError, "'%s' is not supported between instances of %R and %R", opstrings[op], Py_TYPE(left),
//     Py_TYPE(right)
//   );
// }

struct TxOutputArray {
  struct BtcK_TransactionOutput const** data;
  size_t size;
};

static int convert_tx_outputs(PyObject* obj, struct TxOutputArray* out)
{
  if (Py_IsNone(obj)) {
    out->data = NULL;
    out->size = 0;
    return 1;
  }

  PyObject* seq = PySequence_Fast(obj, "spent_outputs must be a sequence");
  if (seq == NULL) {
    return 0;
  }

  Py_ssize_t const size = PySequence_Fast_GET_SIZE(seq);
  struct BtcK_TransactionOutput const** data =
    PyMem_New(struct BtcK_TransactionOutput const*, size);
  if (data == NULL) {
    Py_DECREF(seq);
    PyErr_NoMemory();
    return 0;
  }

  for (Py_ssize_t idx = 0; idx < size; ++idx) {
    PyObject* item = PySequence_Fast_GET_ITEM(seq, idx);
    if (!PyObject_TypeCheck(item, &TransactionOutput_Type)) {
      PyErr_Format(
        PyExc_TypeError, "spent_outputs[%zd] is not a TransactionOutput", idx);
      PyMem_Free((void*)data);
      Py_DECREF(seq);
      return 0;
    }
    data[idx] = TransactionOutput_GetImpl(item);
  }

  out->data = data;
  out->size = (size_t)size;
  Py_DECREF(seq);
  return 1;
}

PyDoc_STRVAR(verify_script_doc, "TODO: Write docstring");
static PyObject* verify_script(
  PyObject* Py_UNUSED(self), PyObject* args, PyObject* kwargs)
{
  static char* kwlist[] = {
    "script_pubkey", "amount", "tx_to", "spent_outputs",
    "input_index",   "flags",  NULL,
  };

  PyObject* script_pubkey = NULL;
  int64_t amount = 0;
  PyObject* tx_to = NULL;
  struct TxOutputArray spent_outputs = {};
  unsigned int input_index = 0;
  PyObject* flags = NULL;

  if (!PyArg_ParseTupleAndKeywords(
        args, kwargs, "O!LO!|O&IO!", kwlist, &ScriptPubkey_Type, &script_pubkey,
        &amount, &Transaction_Type, &tx_to, convert_tx_outputs, &spent_outputs,
        &input_index, &VerificationFlags_Type, &flags)) {
    return NULL;
  }

  struct BtcK_Error* error = NULL;
  int const result = BtcK_Verify(
    ScriptPubkey_GetImpl(script_pubkey), amount, Transaction_GetImpl(tx_to),
    spent_outputs.data, spent_outputs.size, input_index,
    VerificationFlags_GetImpl(flags), &error);
  if (error != NULL) {
    return SetError(error);
  }

  return PyBool_FromLong(result);
}

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

static PyMethodDef methods[] = {
  {
    "verify_script",
    (PyCFunction)verify_script,
    METH_VARARGS | METH_KEYWORDS,
    verify_script_doc,
  },
  {},
};

static PyModuleDef_Slot slots[] = {
  {Py_mod_exec, exec},
  {},
};

static PyModuleDef module = {
  .m_base = PyModuleDef_HEAD_INIT,
  .m_name = "btck",
  .m_doc = "btck module documentation",
  .m_methods = methods,
  .m_slots = slots,
};

PyMODINIT_FUNC PyInit_btck(void);
PyMODINIT_FUNC PyInit_btck(void)
{
  return PyModuleDef_Init(&module);
}
