// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

import Foundation
@_implementationOnly import btck_c

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

    internal init(ptr: OpaquePointer) {
        self.ptr = ptr
    }

    deinit {
        BtcK_Block_Free(ptr)
    }

    public var data: Data {
        var buffer = Data()
        let callback: @convention(c) (UnsafeRawPointer?, size_t, UnsafeMutableRawPointer?) -> Int32 = { bytes, sz, userdata in
            guard let bytes, let userdata else { return -1 }
            let dataPtr = userdata.assumingMemoryBound(to: Data.self)
            dataPtr.pointee.append(bytes.assumingMemoryBound(to: UInt8.self), count: sz)
            return 0
        }
        let result = BtcK_Block_ToBytes(ptr, writeCallback, &buffer)
        guard result == 0 else { return Data() }
        return buffer
    }

    public var hash: BlockHash {
        var out = BlockHash()
        BtcK_Block_GetHash(ptr, &out.raw)
        return out
    }
}

public class Chain {
    internal var ptr: OpaquePointer

    internal init(ptr: OpaquePointer) {
        self.ptr = ptr
    }

    deinit {
        BtcK_Chain_Free(ptr)
    }

    public func find(hash: BlockHash) -> Index {
        withUnsafePointer(to: hash.raw) { ptr in
            BtcK_Chain_FindBlock(self.ptr, ptr)
        }
    }
}
