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
        guard let output = BtcK_Transaction_GetOutput(ptr, position) else {
            fatalError("Index out of bounds")
        }
        return TransactionOutput(ptr: output)
    }
}

extension Transaction: CustomStringConvertible {
    public var description: String {
        var len: size_t = 0
        guard let cstr = BtcK_Transaction_ToString(ptr, &len) else { return "" }
        let data = Data(bytes: cstr, count: Int(len))
        return String(data: data, encoding: .utf8) ?? ""
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
        guard let output = BtcK_Block_GetTransaction(ptr, position) else {
            fatalError("Index out of bounds")
        }
        return Transaction(ptr: output)
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
        guard let output = BtcK_Chain_GetBlock(ptr, position) else {
            fatalError("Index out of bounds")
        }
        return Block(ptr: output)
    }
}
