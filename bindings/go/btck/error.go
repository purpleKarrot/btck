// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

package btck

/*
#cgo LDFLAGS: -lbtck
#include <stdlib.h>
#include <btck/btck.h>
*/
import "C"
import "strconv"

type Error struct {
	Code    int
	Domain  string
	Message string
}

func (e *Error) Error() string {
	return "[" + e.Domain + "] Code " + strconv.Itoa(e.Code) + ": " + e.Message
}

func newError(err *C.struct_BtcK_Error) error {
	new := &Error{
		Code:    int(C.BtcK_Error_Code(err)),
		Domain:  C.GoString(C.BtcK_Error_Message(err)),
		Message: C.GoString(C.BtcK_Error_Message(err)),
	}
	C.BtcK_Error_Free(err)
	return new
}
