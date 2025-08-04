//  Copyright (c) 2025-present The Bitcoin Core developers
//  Distributed under the MIT software license, see the accompanying
//  file COPYING or http://www.opensource.org/licenses/mit-license.php.

import Foundation

typealias EntryPoint = @convention(thin) @Sendable (
    _ configurationJSON: UnsafeRawBufferPointer?,
    _ recordHandler: @escaping @Sendable (_ recordJSON: UnsafeRawBufferPointer) -> Void
) async throws -> Bool

@_extern(c, "swt_abiv0_getEntryPoint")
func swt_abiv0_getEntryPoint() -> UnsafeRawPointer

@main
struct Runner {
    static func main() async throws {
        nonisolated(unsafe) let configurationJSON: UnsafeRawBufferPointer? = nil
        let recordHandler: @Sendable (UnsafeRawBufferPointer) -> Void = { _ in }

        let entryPoint = unsafeBitCast(swt_abiv0_getEntryPoint(), to: EntryPoint.self)

        if try await entryPoint(configurationJSON, recordHandler) {
            exit(EXIT_SUCCESS)
        } else {
            exit(EXIT_FAILURE)
        }
    }
}
