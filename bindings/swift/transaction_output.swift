// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

import Foundation
@_implementationOnly import btck_c

public protocol TransactionOutputProtocol: AnyObject {
    var ptr: OpaquePointer { get }
}

public class TransactionOutput: TransactionOutputProtocol {
    public internal(set) var ptr: OpaquePointer

    public init(amount: Int64, scriptPubkey: ScriptPubkey) throws {
        var err: OpaquePointer? = nil
        let ptr = BtcK_TransactionOutput_New(amount, scriptPubkey.ptr, &err)
        if let err {
            throw BtcKError(err)
        }
        self.ptr = ptr!
    }

    internal init(ptr: OpaquePointer) {
        self.ptr = ptr
    }

    deinit {
        BtcK_TransactionOutput_Free(ptr)
    }
}

public class TransactionOutputRef: TransactionOutputProtocol {
    public let ptr: OpaquePointer

    internal init(ptr: OpaquePointer) {
        self.ptr = ptr
    }
}

public extension TransactionOutputProtocol {
    var amount: Int64 {
        BtcK_TransactionOutput_GetAmount(self.ptr)
    }

    var scriptPubkey: ScriptPubkeyRef {
        ScriptPubkeyRef(ptr: BtcK_TransactionOutput_GetScriptPubkey(self.ptr))
    }
}
