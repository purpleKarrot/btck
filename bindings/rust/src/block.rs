// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

pub struct Block {
    pub(crate) ptr: *mut crate::ffi::BtcK_Block,
}

pub struct BlockTransactions<'a> {
    block: &'a Block,
    idx: usize,
    count: usize,
}

impl Block {
    pub fn new(raw: &[u8]) -> Result<Self, crate::error::Error> {
        let mut err: *mut crate::ffi::BtcK_Error = std::ptr::null_mut();
        let ptr = unsafe { crate::ffi::BtcK_Block_New(raw.as_ptr() as _, raw.len(), &mut err) };
        if !err.is_null() {
            Err(crate::error::Error { err })
        } else {
            Ok(Block { ptr })
        }
    }

    pub fn transactions(&self) -> BlockTransactions<'_> {
        let count = unsafe { crate::ffi::BtcK_Block_CountTransactions(self.ptr) };
        BlockTransactions {
            block: self,
            idx: 0,
            count,
        }
    }
}

impl Drop for Block {
    fn drop(&mut self) {
        unsafe { crate::ffi::BtcK_Block_Free(self.ptr) }
    }
}

impl<'a> Iterator for BlockTransactions<'a> {
    type Item = crate::transaction::Transaction;
    fn next(&mut self) -> Option<Self::Item> {
        if self.idx < self.count {
            let ptr = unsafe { crate::ffi::BtcK_Block_GetTransaction(self.block.ptr, self.idx) };
            self.idx += 1;
            if ptr.is_null() {
                None
            } else {
                Some(crate::transaction::Transaction { ptr })
            }
        } else {
            None
        }
    }
}
