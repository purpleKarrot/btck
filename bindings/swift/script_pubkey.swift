// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

import Foundation
@_implementationOnly import btck_c

public protocol ScriptPubkeyProtocol: AnyObject {
    var ptr: OpaquePointer { get }
}

extension ScriptPubkeyProtocol {
    public static func == (left: Self, right: Self) -> Bool {
        BtcK_ScriptPubkey_Equal(left.ptr, right.ptr) != 0
    }
}

public class ScriptPubkey: ScriptPubkeyProtocol, Equatable {
    public internal(set) var ptr: OpaquePointer

    public init(raw: Data) throws {
        var err: OpaquePointer? = nil
        let ptr = raw.withUnsafeBytes { buf in
            BtcK_ScriptPubkey_New(buf.baseAddress, raw.count, &err)
        }
        if let err {
            throw BtcKError(err)
        }
        self.ptr = ptr!
    }

    internal init(ptr: OpaquePointer) {
        self.ptr = ptr
    }

    deinit {
        BtcK_ScriptPubkey_Free(ptr)
    }
}

public class ScriptPubkeyRef: ScriptPubkeyProtocol, Equatable {
    public let ptr: OpaquePointer

    internal init(ptr: OpaquePointer) {
        self.ptr = ptr
    }
}

public extension ScriptPubkeyProtocol {
    func copy() throws -> ScriptPubkey {
        var err: OpaquePointer? = nil
        let ptr = BtcK_ScriptPubkey_Copy(self.ptr, &err)
        if let err {
            throw BtcKError(err)
        }
        return ScriptPubkey(ptr: ptr!)
    }

    var data: Data {
        var buffer = Data()
        let callback: @convention(c) (UnsafeRawPointer?, size_t, UnsafeMutableRawPointer?) -> Int32 = { bytes, sz, userdata in
            guard let bytes, let userdata else { return -1 }
            let dataPtr = userdata.assumingMemoryBound(to: Data.self)
            dataPtr.pointee.append(bytes.assumingMemoryBound(to: UInt8.self), count: sz)
            return 0
        }
        guard BtcK_ScriptPubkey_ToBytes(ptr, callback, &buffer) == 0 else {
            return Data()
        }
        return buffer
    }
}
