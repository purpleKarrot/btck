// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

import Foundation
@_implementationOnly import btck_c

extension ScriptPubkey: Equatable {
    public static func == (left: ScriptPubkey, right: ScriptPubkey) -> Bool {
        return btck_ScriptPubkey_Equal(left.ptr, right.ptr)
    }
}

extension Transaction: RandomAccessCollection {
    public typealias Index = Int
    public typealias Element = TransactionOutput

    public var startIndex: Int { 0 }
    public var endIndex: Int { btck_Transaction_GetSize(ptr) }

    public func index(after i: Int) -> Int { i + 1 }
    public func index(before i: Int) -> Int { i - 1 }

    public subscript(position: Int) -> TransactionOutput {
        guard let output = btck_Transaction_At(ptr, position) else {
            fatalError("Index out of bounds")
        }
        return TransactionOutput(owned: output)
    }
}

extension Transaction: CustomStringConvertible {
    public var description: String {
        var len: size_t = 0
        guard let cstr = btck_Transaction_ToString(ptr, &len) else { return "" }
        let data = Data(bytes: cstr, count: Int(len))
        return String(data: data, encoding: .utf8) ?? ""
    }
}

extension Block: RandomAccessCollection {
    public typealias Index = Int
    public typealias Element = Transaction

    public var startIndex: Int { 0 }
    public var endIndex: Int { btck_Block_GetSize(ptr) }

    public func index(after i: Int) -> Int { i + 1 }
    public func index(before i: Int) -> Int { i - 1 }

    public subscript(position: Int) -> Transaction {
        guard let output = btck_Block_At(ptr, position) else {
            fatalError("Index out of bounds")
        }
        return Transaction(owned: output)
    }
}

extension Chain: RandomAccessCollection {
    public typealias Index = Int
    public typealias Element = Block

    public var startIndex: Int { 0 }
    public var endIndex: Int { btck_Block_GetSize(ptr) }

    public func index(after i: Int) -> Int { i + 1 }
    public func index(before i: Int) -> Int { i - 1 }

    public subscript(position: Int) -> Block {
        guard let output = btck_Chain_At(ptr, position) else {
            fatalError("Index out of bounds")
        }
        return Block(owned: output)
    }
}
