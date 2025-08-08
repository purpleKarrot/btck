// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

import Foundation
@_implementationOnly import btck_c

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
