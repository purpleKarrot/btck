// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

pub struct TransactionOutput {
    pub(crate) ptr: *mut crate::ffi::BtcK_TransactionOutput,
}

impl TransactionOutput {
    pub fn new(
        amount: i64,
        script_pubkey: &crate::script_pubkey::ScriptPubkey,
    ) -> Result<Self, crate::error::Error> {
        let mut err: *mut crate::ffi::BtcK_Error = std::ptr::null_mut();
        let ptr =
            unsafe { crate::ffi::BtcK_TransactionOutput_New(amount, script_pubkey.ptr, &mut err) };
        if !err.is_null() {
            Err(crate::error::Error { err })
        } else {
            Ok(TransactionOutput { ptr })
        }
    }

    pub fn amount(&self) -> i64 {
        unsafe { crate::ffi::BtcK_TransactionOutput_GetAmount(self.ptr) }
    }
}

impl Drop for TransactionOutput {
    fn drop(&mut self) {
        unsafe {
            crate::ffi::BtcK_TransactionOutput_Release(self.ptr);
        }
    }
}
