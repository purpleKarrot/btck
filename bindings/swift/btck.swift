// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

import Foundation
@_implementationOnly import btck_c

// MARK: - ScriptPubkey

public class ScriptPubkey {
    internal var ptr: OpaquePointer

    public init?(raw: Data) {
        guard let p = btck_ScriptPubkey_New((raw as NSData).bytes, raw.count) else { return nil }
        self.ptr = p
    }

    internal init(owned ptr: OpaquePointer) {
        self.ptr = ptr
    }

    internal init(unowned ptr: OpaquePointer) {
        self.ptr = btck_ScriptPubkey_Retain(ptr)
    }

    public func asBytes() -> Data {
        var len: size_t = 0
        guard let bytes = btck_ScriptPubkey_AsBytes(ptr, &len) else { return Data() }
        return Data(bytes: bytes, count: len)
    }

    deinit {
        btck_ScriptPubkey_Release(ptr)
    }
}

// MARK: - TransactionOutput

public class TransactionOutput {
    internal var ptr: OpaquePointer

    public init?(amount: Int64, scriptPubkey: ScriptPubkey) {
        guard let ptr = btck_TransactionOutput_New(amount, scriptPubkey.ptr) else { return nil }
        self.ptr = ptr
    }

    internal init(owned ptr: OpaquePointer) {
        self.ptr = ptr
    }

    internal init(unowned ptr: OpaquePointer) {
        self.ptr = btck_TransactionOutput_Retain(ptr)
    }

    public var amount: Int64 {
        return btck_TransactionOutput_GetAmount(ptr)
    }

    public var scriptPubkey: ScriptPubkey {
        let sp = btck_TransactionOutput_GetScriptPubkey(ptr)!
        return ScriptPubkey(owned: sp)
    }

    deinit {
        btck_TransactionOutput_Release(ptr)
    }
}

// MARK: - Transaction

public class Transaction {
    internal var ptr: OpaquePointer

    public init?(raw: Data) {
        guard let p = btck_Transaction_New((raw as NSData).bytes, raw.count) else { return nil }
        self.ptr = p
    }

    internal init(owned ptr: OpaquePointer) {
        self.ptr = ptr
    }

    internal init(unowned ptr: OpaquePointer) {
        self.ptr = btck_Transaction_Retain(ptr)
    }

    public func asBytes() -> Data {
        var len: size_t = 0
        guard let bytes = btck_Transaction_AsBytes(ptr, &len) else { return Data() }
        return Data(bytes: bytes, count: len)
    }

    deinit {
        btck_Transaction_Release(ptr)
    }
}

// MARK: - BlockHash

public struct BlockHash {
    internal var raw: btck_BlockHash

    public static let size = Int(btck_BlockHash_SIZE)

    public init() {
        self.raw = .init()
    }

    public init?(data: Data) {
        guard data.count == BlockHash.size else { return nil }
        var hash = btck_BlockHash()
        data.withUnsafeBytes { buf in
            btck_BlockHash_Init(&hash, buf.baseAddress, BlockHash.size)
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

    public init?(raw: Data) {
        guard let p = btck_Block_New((raw as NSData).bytes, raw.count) else { return nil }
        self.ptr = p
    }

    internal init(owned ptr: OpaquePointer) {
        self.ptr = ptr
    }

    internal init(unowned ptr: OpaquePointer) {
        self.ptr = btck_Block_Retain(ptr)
    }

    deinit {
        btck_Block_Release(ptr)
    }

    public var data: Data {
        var len: size_t = 0
        guard let bytes = btck_Block_AsBytes(ptr, &len) else { return Data() }
        return Data(bytes: bytes, count: len)
    }

    public var hash: BlockHash {
        var out = BlockHash()
        btck_Block_GetHash(ptr, &out.raw)
        return out
    }
}

public class Chain {
    internal var ptr: OpaquePointer

    internal init(owned ptr: OpaquePointer) {
        self.ptr = ptr
    }

    internal init(unowned ptr: OpaquePointer) {
        self.ptr = btck_Chain_Retain(ptr)
    }

    deinit {
        btck_Chain_Release(ptr)
    }

    public func find(hash: BlockHash) -> Index {
        withUnsafePointer(to: hash.raw) { ptr in
            btck_Chain_Find(self.ptr, ptr);
        }
    }
}
