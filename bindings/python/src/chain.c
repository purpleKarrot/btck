// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chain.h"

#include <btck/btck.h>

#include <stddef.h>

#include "_slice.h"
#include "block.h"

struct Self {
  PyObject_HEAD
  struct BtcK_Chain* impl;
};

static void dealloc(struct Self* self);
static PyObject* new(PyTypeObject* type, PyObject* args, PyObject* kwargs);
static PyObject* get_blocks_slice(struct Self const* self, void* closure);

static Py_ssize_t num_blocks(struct Self const* self);
static PyObject* get_block(struct Self* self, Py_ssize_t idx);

static PyGetSetDef getset[] = {
  {"blocks", (getter)get_blocks_slice, NULL, "", NULL},
  {},
};

static PySequenceMethods blocks_as_sequence = {
  .sq_length = (lenfunc)num_blocks,
  .sq_item = (ssizeargfunc)get_block,
};

static PyMappingMethods blocks_as_mapping = {
  .mp_length = (lenfunc)num_blocks,
  .mp_subscript = Slice_subscript,
};

PyTypeObject Chain_Type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "btck.Chain",
  .tp_doc = "Chain object",
  .tp_basicsize = sizeof(struct Self),
  .tp_dealloc = (destructor)dealloc,
  .tp_flags = Py_TPFLAGS_DEFAULT,
  .tp_new = new,
  .tp_getset = getset,
};

PyTypeObject Chain_BlocksSlice_Type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "btck._Slice[Block]",
  .tp_basicsize = sizeof(struct Self),
  .tp_dealloc = (destructor)dealloc,
  .tp_flags = Py_TPFLAGS_DEFAULT,
  .tp_as_sequence = &blocks_as_sequence,
  .tp_as_mapping = &blocks_as_mapping,
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

static PyObject* get_blocks_slice(
  struct Self const* self, void* Py_UNUSED(closure))
{
  struct Self* slice = PyObject_New(struct Self, &Chain_BlocksSlice_Type);
  if (slice == NULL) {
    return NULL;
  }
  slice->impl = BtcK_Chain_Retain(self->impl);
  return (PyObject*)slice;
}

static Py_ssize_t num_blocks(struct Self const* self)
{
  return (Py_ssize_t)BtcK_Chain_NumBlocks(self->impl);
}

static PyObject* get_block(struct Self* self, Py_ssize_t idx)
{
  if (idx < 0 || idx >= num_blocks(self)) {
    PyErr_SetString(PyExc_IndexError, "index out of range");
    return NULL;
  }
  return Block_New(BtcK_Chain_GetBlock(self->impl, idx));
}

//   BlockIndex GetBlockIndexByHash(kernel_BlockHash * block_hash) const noexcept
//   {
//     return kernel_get_block_index_from_hash(
//         m_context.m_context.get(), m_chainman, block_hash);
//   }
