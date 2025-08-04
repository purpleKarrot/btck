// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

import Foundation
@_implementationOnly import btck_c

// MARK: - Error

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

// MARK: - ScriptPubkey

public class ScriptPubkey {
    internal var ptr: OpaquePointer

    public init(raw: Data) throws {
        var err: OpaquePointer? = nil
        let ptr = raw.withUnsafeBytes({ buf in
            BtcK_ScriptPubkey_New(buf.baseAddress, raw.count, &err)
        })
        if let err {
            throw BtcKError(err)
        }
        self.ptr = ptr!
    }

    internal init(owned ptr: OpaquePointer) {
        self.ptr = ptr
    }

    internal init(unowned ptr: OpaquePointer) {
        self.ptr = BtcK_ScriptPubkey_Retain(ptr)
    }

    public func asBytes() -> Data {
        var len: size_t = 0
        guard let bytes = BtcK_ScriptPubkey_AsBytes(ptr, &len) else { return Data() }
        return Data(bytes: bytes, count: len)
    }

    deinit {
        BtcK_ScriptPubkey_Release(ptr)
    }
}

// MARK: - TransactionOutput

public class TransactionOutput {
    internal var ptr: OpaquePointer

    public init?(amount: Int64, scriptPubkey: ScriptPubkey) throws {
        var err: OpaquePointer? = nil
        let ptr = BtcK_TransactionOutput_New(amount, scriptPubkey.ptr, &err)
        if let err {
            throw BtcKError(err)
        }
        self.ptr = ptr!
    }

    internal init(owned ptr: OpaquePointer) {
        self.ptr = ptr
    }

    internal init(unowned ptr: OpaquePointer) {
        self.ptr = BtcK_TransactionOutput_Retain(ptr)
    }

    public var amount: Int64 {
        return BtcK_TransactionOutput_GetAmount(ptr)
    }

    public var scriptPubkey: ScriptPubkey {
        get throws {
            var err: OpaquePointer? = nil
            let sp = BtcK_TransactionOutput_GetScriptPubkey(self.ptr, &err)
            if let err {
                throw BtcKError(err)
            }
            return ScriptPubkey(owned: sp!)
        }
    }

    deinit {
        BtcK_TransactionOutput_Release(ptr)
    }
}

// MARK: - Transaction

public class Transaction {
    internal var ptr: OpaquePointer

    public init(raw: Data) throws {
        var err: OpaquePointer? = nil
        let ptr = raw.withUnsafeBytes({ buf in
            BtcK_Transaction_New(buf.baseAddress, raw.count, &err)
        })
        if let err {
            throw BtcKError(err)
        }
        self.ptr = ptr!
    }

    internal init(owned ptr: OpaquePointer) {
        self.ptr = ptr
    }

    internal init(unowned ptr: OpaquePointer) {
        self.ptr = BtcK_Transaction_Retain(ptr)
    }

    public func asBytes() -> Data {
        var len: size_t = 0
        guard let bytes = BtcK_Transaction_AsBytes(ptr, &len) else { return Data() }
        return Data(bytes: bytes, count: len)
    }

    deinit {
        BtcK_Transaction_Release(ptr)
    }
}

// MARK: - BlockHash

public struct BlockHash {
    internal var raw: BtcK_BlockHash

    public static let size = Int(BtcK_BlockHash_SIZE)

    public init() {
        self.raw = .init()
    }

    public init?(data: Data) {
        guard data.count == BlockHash.size else { return nil }
        var hash = BtcK_BlockHash()
        data.withUnsafeBytes { buf in
            BtcK_BlockHash_Init(&hash, buf.baseAddress, BlockHash.size)
        }
        self.raw = hash
    }

    public var data: Data {
        withUnsafeBytes(of: raw.data) { Data($0) }
    }
}

// MARK: - Block

public class Block {
    internal var ptr: OpaquePointer

    public init(raw: Data) throws {
        var err: OpaquePointer? = nil
        let ptr = raw.withUnsafeBytes({ buf in
            BtcK_Block_New(buf.baseAddress, raw.count, &err)
        })
        if let err {
            throw BtcKError(err)
        }
        self.ptr = ptr!
    }

    internal init(owned ptr: OpaquePointer) {
        self.ptr = ptr
    }

    internal init(unowned ptr: OpaquePointer) {
        self.ptr = BtcK_Block_Retain(ptr)
    }

    deinit {
        BtcK_Block_Release(ptr)
    }

    public var data: Data {
        var len: size_t = 0
        guard let bytes = BtcK_Block_AsBytes(ptr, &len) else { return Data() }
        return Data(bytes: bytes, count: len)
    }

    public var hash: BlockHash {
        var out = BlockHash()
        BtcK_Block_GetHash(ptr, &out.raw)
        return out
    }
}

public class Chain {
    internal var ptr: OpaquePointer

    internal init(owned ptr: OpaquePointer) {
        self.ptr = ptr
    }

    internal init(unowned ptr: OpaquePointer) {
        self.ptr = BtcK_Chain_Retain(ptr)
    }

    deinit {
        BtcK_Chain_Release(ptr)
    }

    public func find(hash: BlockHash) -> Index {
        withUnsafePointer(to: hash.raw) { ptr in
            BtcK_Chain_FindBlock(self.ptr, ptr)
        }
    }
}
