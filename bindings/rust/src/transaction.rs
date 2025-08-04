// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

pub struct Transaction {
    pub(crate) ptr: *mut crate::ffi::BtcK_Transaction,
}

pub struct TransactionOutputs<'a> {
    tx: &'a Transaction,
    idx: usize,
    count: usize,
}

impl Transaction {
    pub fn new(raw: &[u8]) -> Result<Self, crate::error::Error> {
        let mut err: *mut crate::ffi::BtcK_Error = std::ptr::null_mut();
        let ptr =
            unsafe { crate::ffi::BtcK_Transaction_New(raw.as_ptr() as _, raw.len(), &mut err) };
        if !err.is_null() {
            Err(crate::error::Error { err })
        } else {
            Ok(Transaction { ptr })
        }
    }

    pub fn outputs(&self) -> TransactionOutputs<'_> {
        let count = unsafe { crate::ffi::BtcK_Transaction_CountOutputs(self.ptr) };
        TransactionOutputs {
            tx: self,
            idx: 0,
            count,
        }
    }

    pub fn as_bytes(&self) -> Vec<u8> {
        let mut len: usize = 0;
        let data = unsafe { crate::ffi::BtcK_Transaction_AsBytes(self.ptr, &mut len) };
        if data.is_null() || len == 0 {
            Vec::new()
        } else {
            unsafe { std::slice::from_raw_parts(data as *const u8, len).to_vec() }
        }
    }

    pub fn to_string(&self) -> String {
        let mut len: usize = 0;
        let cstr = unsafe { crate::ffi::BtcK_Transaction_ToString(self.ptr, &mut len) };
        if cstr.is_null() || len == 0 {
            String::new()
        } else {
            unsafe {
                std::str::from_utf8_unchecked(std::slice::from_raw_parts(cstr as *const u8, len))
                    .to_string()
            }
        }
    }
}

impl Drop for Transaction {
    fn drop(&mut self) {
        unsafe { crate::ffi::BtcK_Transaction_Release(self.ptr) }
    }
}

impl<'a> Iterator for TransactionOutputs<'a> {
    type Item = crate::transaction_output::TransactionOutput;
    fn next(&mut self) -> Option<Self::Item> {
        if self.idx < self.count {
            let ptr = unsafe { crate::ffi::BtcK_Transaction_GetOutput(self.tx.ptr, self.idx) };
            self.idx += 1;
            if ptr.is_null() {
                None
            } else {
                Some(crate::transaction_output::TransactionOutput { ptr })
            }
        } else {
            None
        }
    }
}
