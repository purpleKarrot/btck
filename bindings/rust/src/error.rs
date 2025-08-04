// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

pub struct Error {
    pub(crate) err: *mut crate::ffi::BtcK_Error,
}

impl Error {
    pub fn code(&self) -> i32 {
        unsafe { crate::ffi::BtcK_Error_Code(self.err) }
    }

    pub fn domain(&self) -> String {
        unsafe {
            std::ffi::CStr::from_ptr(crate::ffi::BtcK_Error_Domain(self.err))
                .to_string_lossy()
                .into_owned()
        }
    }

    pub fn message(&self) -> String {
        unsafe {
            std::ffi::CStr::from_ptr(crate::ffi::BtcK_Error_Message(self.err))
                .to_string_lossy()
                .into_owned()
        }
    }
}

impl std::fmt::Display for Error {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "[{}:{}] {}", self.domain(), self.code(), self.message())
    }
}

impl std::fmt::Debug for Error {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("Error")
            .field("code", &self.code())
            .field("domain", &self.domain())
            .field("message", &self.message())
            .finish()
    }
}

impl std::error::Error for Error {}

impl Drop for Error {
    fn drop(&mut self) {
        unsafe {
            crate::ffi::BtcK_Error_Free(self.err);
        }
    }
}
