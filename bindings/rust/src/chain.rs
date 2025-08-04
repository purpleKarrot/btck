// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

pub struct Chain {
    ptr: *mut crate::ffi::BtcK_Chain,
}

pub struct ChainBlocks<'a> {
    chain: &'a Chain,
    idx: usize,
    count: usize,
}

impl Chain {
    pub fn blocks(&self) -> ChainBlocks<'_> {
        let count = unsafe { crate::ffi::BtcK_Chain_CountBlocks(self.ptr) };
        ChainBlocks {
            chain: self,
            idx: 0,
            count,
        }
    }
}

impl Drop for Chain {
    fn drop(&mut self) {
        unsafe { crate::ffi::BtcK_Chain_Release(self.ptr) }
    }
}

impl<'a> Iterator for ChainBlocks<'a> {
    type Item = crate::block::Block;
    fn next(&mut self) -> Option<Self::Item> {
        if self.idx < self.count {
            let ptr = unsafe { crate::ffi::BtcK_Chain_GetBlock(self.chain.ptr, self.idx) };
            self.idx += 1;
            if ptr.is_null() {
                None
            } else {
                Some(crate::block::Block { ptr })
            }
        } else {
            None
        }
    }
}
