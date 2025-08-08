// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

import Foundation
@_implementationOnly import btck_c

public class BtcKError: Error {
    var ptr: OpaquePointer

    internal init(_ err: OpaquePointer) {
        self.ptr = err
    }

    public var code: Int {
        Int(BtcK_Error_Code(self.ptr))
    }

    public var domain: String {
        String(cString: BtcK_Error_Domain(self.ptr))
    }

    public var message: String {
        String(cString: BtcK_Error_Message(self.ptr))
    }

    deinit {
        BtcK_Error_Free(self.ptr)
    }
}
