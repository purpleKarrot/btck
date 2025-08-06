// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

import Foundation
@_implementationOnly import btck_c

extension ScriptPubkey: Equatable {
    public static func == (left: ScriptPubkey, right: ScriptPubkey) -> Bool {
        BtcK_ScriptPubkey_Equal(left.ptr, right.ptr) != 0
    }
}

extension Transaction: RandomAccessCollection {
    public typealias Index = Int
    public typealias Element = TransactionOutput

    public var startIndex: Int { 0 }
    public var endIndex: Int { BtcK_Transaction_CountOutputs(ptr) }

    public func index(after i: Int) -> Int { i + 1 }
    public func index(before i: Int) -> Int { i - 1 }

    public subscript(position: Int) -> TransactionOutput {
        var err: OpaquePointer? = nil
        let ptr = BtcK_Transaction_GetOutput(ptr, position, &err)
        if let err {
            fatalError("\(BtcKError(err))")
        }
        return TransactionOutput(ptr: ptr!)
    }
}

extension Transaction: CustomStringConvertible {
    public var description: String {
        let len = BtcK_Transaction_ToString(ptr, nil, 0)
        guard len > 0 else { return "" }
        let buf = UnsafeMutablePointer<CChar>.allocate(capacity: Int(len) + 1)
        defer { buf.deallocate() }
        let written = BtcK_Transaction_ToString(ptr, buf, Int(len) + 1)
        guard written >= 0 else { return "" }
        return String(cString: buf)
    }
}

extension Block: RandomAccessCollection {
    public typealias Index = Int
    public typealias Element = Transaction

    public var startIndex: Int { 0 }
    public var endIndex: Int { BtcK_Block_CountTransactions(ptr) }

    public func index(after i: Int) -> Int { i + 1 }
    public func index(before i: Int) -> Int { i - 1 }

    public subscript(position: Int) -> Transaction {
        var err: OpaquePointer? = nil
        let ptr = BtcK_Block_GetTransaction(ptr, position, &err)
        if let err {
            fatalError("\(BtcKError(err))")
        }
        return Transaction(ptr: ptr!)
    }
}

extension Chain: RandomAccessCollection {
    public typealias Index = Int
    public typealias Element = Block

    public var startIndex: Int { 0 }
    public var endIndex: Int { BtcK_Chain_CountBlocks(ptr) }

    public func index(after i: Int) -> Int { i + 1 }
    public func index(before i: Int) -> Int { i - 1 }

    public subscript(position: Int) -> Block {
        var err: OpaquePointer? = nil
        let ptr = BtcK_Chain_GetBlock(ptr, position, &err)
        if let err {
            fatalError("\(BtcKError(err))")
        }
        return Block(ptr: ptr!)
    }
}
