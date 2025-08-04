// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

pub struct ScriptPubkey {
    pub(crate) ptr: *mut crate::ffi::BtcK_ScriptPubkey,
}

impl ScriptPubkey {
    pub fn new(raw: &[u8]) -> Result<Self, crate::error::Error> {
        let mut err: *mut crate::ffi::BtcK_Error = std::ptr::null_mut();
        let ptr = unsafe {
            crate::ffi::BtcK_ScriptPubkey_New(
                raw.as_ptr() as *const std::ffi::c_void,
                raw.len(),
                &mut err,
            )
        };
        if !err.is_null() {
            Err(crate::error::Error { err })
        } else {
            Ok(ScriptPubkey { ptr })
        }
    }

    pub fn as_bytes(&self) -> Vec<u8> {
        let mut len: usize = 0;
        let data = unsafe { crate::ffi::BtcK_ScriptPubkey_AsBytes(self.ptr, &mut len) };
        if data.is_null() || len == 0 {
            Vec::new()
        } else {
            unsafe { std::slice::from_raw_parts(data as *const u8, len).to_vec() }
        }
    }
}

impl Drop for ScriptPubkey {
    fn drop(&mut self) {
        unsafe { crate::ffi::BtcK_ScriptPubkey_Release(self.ptr) }
    }
}
