// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

package btck

import "unsafe"

func ptrToSlice(slice []byte) unsafe.Pointer {
	if len(slice) > 0 {
		return unsafe.Pointer(&slice[0])
	}
	return unsafe.Pointer(nil)
}
