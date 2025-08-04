// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chain.h"

#include <btck/btck.h>

#include <stddef.h>

#include "_slice.h"
#include "block.h"
#include "block_hash.h"

struct Self {
  PyObject_HEAD
  struct BtcK_Chain* impl;
};

static void dealloc(struct Self* self);
static PyObject* new(PyTypeObject* type, PyObject* args, PyObject* kwargs);
static PyObject* get_blocks(struct Self const* self, void* closure);

static Py_ssize_t num_blocks(struct Self const* self);
static PyObject* get_block(struct Self* self, Py_ssize_t idx);
static PyObject* find_block(struct Self const* self, PyObject* args);
static PyObject* block_index(struct Self const* self, PyObject* args);

static PyGetSetDef getset[] = {
  {"blocks", (getter)get_blocks, NULL, "", NULL},
  {},
};

static PyMethodDef blocks_methods[] = {
  {"index", (PyCFunction)block_index, METH_VARARGS, ""},
  {"find", (PyCFunction)find_block, METH_VARARGS, ""},
  {},
};

PyTypeObject Chain_Type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "btck.Chain",
  .tp_doc = "Chain object",
  .tp_basicsize = sizeof(struct Self),
  .tp_dealloc = (destructor)dealloc,
  .tp_flags = Py_TPFLAGS_DEFAULT,
  .tp_methods = blocks_methods, // TODO: Would be nicer at blocks
  .tp_new = new,
  .tp_getset = getset,
};

static void dealloc(struct Self* self)
{
  BtcK_Chain_Release(self->impl);
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* new(
  PyTypeObject* Py_UNUSED(type), PyObject* args, PyObject* kwargs)
{
  //   ChainMan(
  //       Context const& context, ChainstateManagerOptions const& chainman_opts)
  //       noexcept
  //     : m_chainman{kernel_chainstate_manager_create(
  //           context.m_context.get(), chainman_opts.m_options.get())}
  //     , m_context{context}
  //   {}
  return NULL;
}

//   bool ImportBlocks(std::span<std::string const> const paths) const noexcept
//   {
//     std::vector<char const*> c_paths;
//     std::vector<size_t> c_paths_lens;
//     c_paths.reserve(paths.size());
//     c_paths_lens.reserve(paths.size());
//     for (auto const& path : paths) {
//       c_paths.push_back(path.c_str());
//       c_paths_lens.push_back(path.length());
//     }

//     return kernel_import_blocks(
//         m_context.m_context.get(),
//         m_chainman,
//         c_paths.data(),
//         c_paths_lens.data(),
//         c_paths.size());
//   }

//   bool ProcessBlock(Block const& block, bool* new_block) const noexcept
//   {
//     return kernel_chainstate_manager_process_block(
//         m_context.m_context.get(), m_chainman, block.m_block.get(), new_block);
//   }

static PyObject* block_item(struct Self* self, Py_ssize_t idx)
{
  return Block_New(BtcK_Chain_GetBlock(self->impl, idx));
}

static PyObject* get_blocks(struct Self const* self, void* Py_UNUSED(closure))
{
  Py_ssize_t const length = (Py_ssize_t)BtcK_Chain_CountBlocks(self->impl);
  return Slice_New((PyObject*)self, length, (ssizeargfunc)block_item);
}

static PyObject* block_index(struct Self const* self, PyObject* args)
{
  PyObject* block_hash = NULL;
  if (!PyArg_ParseTuple(args, "O!", &BlockHash_Type, &block_hash)) {
    return NULL;
  }

  ptrdiff_t const index =
    BtcK_Chain_FindBlock(self->impl, BlockHash_GetImpl(block_hash));
  if (index < 0) {
    PyErr_SetString(PyExc_ValueError, "value not found in blocks");
    return NULL;
  }

  return PyLong_FromSsize_t(index);
}

static PyObject* find_block(struct Self const* self, PyObject* args)
{
  PyObject* block_hash = NULL;
  if (!PyArg_ParseTuple(args, "O!", &BlockHash_Type, &block_hash)) {
    return NULL;
  }

  ptrdiff_t const index =
    BtcK_Chain_FindBlock(self->impl, BlockHash_GetImpl(block_hash));

  return PyLong_FromSsize_t(index);
}
