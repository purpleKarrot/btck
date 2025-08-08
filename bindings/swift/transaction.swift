// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

import Foundation
@_implementationOnly import btck_c

public protocol TransactionProtocol {
    var ptr: OpaquePointer { get }
}

public extension TransactionProtocol { // RandomAccessCollection {
    typealias Index = Int
    typealias Element = TransactionOutputRef

    var startIndex: Int { 0 }
    var endIndex: Int { BtcK_Transaction_CountOutputs(ptr) }

    func index(after i: Int) -> Int { i + 1 }
    func index(before i: Int) -> Int { i - 1 }

    subscript(position: Int) -> TransactionOutputRef {
        TransactionOutputRef(ptr: BtcK_Transaction_GetOutput(ptr, position))
    }
}

public extension TransactionProtocol { // CustomStringConvertible {
    var description: String {
        let len = BtcK_Transaction_ToString(ptr, nil, 0)
        guard len > 0 else { return "" }
        let buf = UnsafeMutablePointer<CChar>.allocate(capacity: Int(len) + 1)
        defer { buf.deallocate() }
        let written = BtcK_Transaction_ToString(ptr, buf, Int(len) + 1)
        guard written >= 0 else { return "" }
        return String(cString: buf)
    }
}

public class Transaction: TransactionProtocol, RandomAccessCollection, CustomStringConvertible {
    public internal(set) var ptr: OpaquePointer

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

    internal init(ptr: OpaquePointer) {
        self.ptr = ptr
    }

    deinit {
        BtcK_Transaction_Free(ptr)
    }
}

public class TransactionRef: TransactionProtocol, RandomAccessCollection, CustomStringConvertible {
    public let ptr: OpaquePointer

    internal init(ptr: OpaquePointer) {
        self.ptr = ptr
    }
}

public extension TransactionProtocol {
    var data: Data {
        var buffer = Data()
        let callback: @convention(c) (UnsafeRawPointer?, size_t, UnsafeMutableRawPointer?) -> Int32 = { bytes, sz, userdata in
            guard let bytes, let userdata else { return -1 }
            let dataPtr = userdata.assumingMemoryBound(to: Data.self)
            dataPtr.pointee.append(bytes.assumingMemoryBound(to: UInt8.self), count: sz)
            return 0
        }
        let result = BtcK_Transaction_ToBytes(ptr, callback, &buffer)
        guard result == 0 else { return Data() }
        return buffer
    }
}
