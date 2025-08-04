// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#![allow(non_camel_case_types, non_snake_case, non_upper_case_globals)]

use std::os::raw::{c_char, c_int, c_longlong, c_uchar, c_uint, c_void};

#[repr(C)]
pub(crate) struct BtcK_Block;
#[repr(C)]
pub(crate) struct BtcK_Chain;
#[repr(C)]
pub(crate) struct BtcK_ScriptPubkey;
#[repr(C)]
pub(crate) struct BtcK_Transaction;
#[repr(C)]
pub(crate) struct BtcK_TransactionOutput;
#[repr(C)]
pub(crate) struct BtcK_Error;

#[repr(C)]
pub struct BtcK_BlockHash {
    pub data: [c_uchar; 32],
}

pub type BtcK_VerificationError = c_uchar;
pub type BtcK_VerificationFlags = c_uint;
pub type BtcK_ChainType = c_uchar;

#[allow(improper_ctypes)]
#[link(name = "btck")]
extern "C" {

    // Error API

    pub(crate) fn BtcK_Error_New(
        domain: *const c_char,
        code: c_int,
        message: *const c_char,
    ) -> *mut BtcK_Error;
    pub(crate) fn BtcK_Error_Free(error: *mut BtcK_Error);
    pub(crate) fn BtcK_Error_Code(error: *const BtcK_Error) -> c_int;
    pub(crate) fn BtcK_Error_Domain(error: *const BtcK_Error) -> *const c_char;
    pub(crate) fn BtcK_Error_Message(error: *const BtcK_Error) -> *const c_char;

    // ScriptPubkey API

    pub(crate) fn BtcK_ScriptPubkey_New(
        raw: *const c_void,
        len: usize,
        err: *mut *mut BtcK_Error,
    ) -> *mut BtcK_ScriptPubkey;
    pub(crate) fn BtcK_ScriptPubkey_Free(self_: *mut BtcK_ScriptPubkey);
    pub(crate) fn BtcK_ScriptPubkey_Equal(
        left: *const BtcK_ScriptPubkey,
        right: *const BtcK_ScriptPubkey,
    ) -> c_int;
    pub(crate) fn BtcK_ScriptPubkey_AsBytes(
        self_: *const BtcK_ScriptPubkey,
        len: *mut usize,
    ) -> *const c_void;

    // TransactionOutput API

    pub(crate) fn BtcK_TransactionOutput_New(
        amount: c_longlong,
        script_pubkey: *const BtcK_ScriptPubkey,
        err: *mut *mut BtcK_Error,
    ) -> *mut BtcK_TransactionOutput;
    pub(crate) fn BtcK_TransactionOutput_Free(self_: *mut BtcK_TransactionOutput);
    pub(crate) fn BtcK_TransactionOutput_GetAmount(
        self_: *const BtcK_TransactionOutput,
    ) -> c_longlong;
    pub(crate) fn BtcK_TransactionOutput_GetScriptPubkey(
        self_: *const BtcK_TransactionOutput,
        err: *mut *mut BtcK_Error,
    ) -> *mut BtcK_ScriptPubkey;

    // Transaction API

    pub(crate) fn BtcK_Transaction_New(
        raw: *const c_void,
        len: usize,
        err: *mut *mut BtcK_Error,
    ) -> *mut BtcK_Transaction;
    pub(crate) fn BtcK_Transaction_Free(self_: *mut BtcK_Transaction);
    pub(crate) fn BtcK_Transaction_CountOutputs(self_: *const BtcK_Transaction) -> usize;
    pub(crate) fn BtcK_Transaction_GetOutput(
        self_: *const BtcK_Transaction,
        idx: usize,
    ) -> *mut BtcK_TransactionOutput;
    pub(crate) fn BtcK_Transaction_AsBytes(
        self_: *const BtcK_Transaction,
        len: *mut usize,
    ) -> *const c_void;
    pub(crate) fn BtcK_Transaction_ToString(
        self_: *const BtcK_Transaction,
        len: *mut usize,
    ) -> *const c_char;

    // Verification API

    pub(crate) fn BtcK_VerificationError_Message(err: BtcK_VerificationError) -> *const c_char;
    pub(crate) fn BtcK_VerificationFlags_ToString(
        flags: BtcK_VerificationFlags,
        buf: *mut c_char,
        len: usize,
    ) -> c_int;
    pub(crate) fn BtcK_Verify(
        script_pubkey: *const BtcK_ScriptPubkey,
        amount: c_longlong,
        tx_to: *const BtcK_Transaction,
        spent_outputs: *const *const BtcK_TransactionOutput,
        spent_outputs_len: usize,
        input_index: c_uint,
        flags: BtcK_VerificationFlags,
        err: *mut *mut BtcK_Error,
    ) -> c_int;

    // Block API

    pub(crate) fn BtcK_BlockHash_Init(self_: *mut BtcK_BlockHash, raw: *const c_void, len: usize);
    pub(crate) fn BtcK_Block_New(
        raw: *const c_void,
        len: usize,
        err: *mut *mut BtcK_Error,
    ) -> *mut BtcK_Block;
    pub(crate) fn BtcK_Block_Free(self_: *mut BtcK_Block);
    pub(crate) fn BtcK_Block_GetHash(self_: *const BtcK_Block, out: *mut BtcK_BlockHash);
    pub(crate) fn BtcK_Block_CountTransactions(self_: *const BtcK_Block) -> usize;
    pub(crate) fn BtcK_Block_GetTransaction(
        self_: *const BtcK_Block,
        idx: usize,
    ) -> *mut BtcK_Transaction;
    pub(crate) fn BtcK_Block_AsBytes(self_: *const BtcK_Block, len: *mut usize) -> *const c_void;

    // Chain API

    pub(crate) fn BtcK_Chain_New() -> *mut BtcK_Chain;
    pub(crate) fn BtcK_Chain_Free(self_: *mut BtcK_Chain);
    pub(crate) fn BtcK_Chain_CountBlocks(self_: *const BtcK_Chain) -> usize;
    pub(crate) fn BtcK_Chain_GetBlock(self_: *const BtcK_Chain, idx: usize) -> *mut BtcK_Block;
    pub(crate) fn BtcK_Chain_FindBlock(
        self_: *const BtcK_Chain,
        block_hash: *const BtcK_BlockHash,
    ) -> isize;
}
