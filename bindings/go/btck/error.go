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
import (
	"runtime"
	"strconv"
)

type Error struct {
	ptr *C.struct_BtcK_Error
}

func (e *Error) Code() int {
	return int(C.BtcK_Error_Code(e.ptr))
}

func (e *Error) Domain() string {
	return C.GoString(C.BtcK_Error_Domain(e.ptr))
}

func (e *Error) Message() string {
	return C.GoString(C.BtcK_Error_Message(e.ptr))
}

func (e *Error) Error() string {
	return "[" + e.Domain() + "] Code " + strconv.Itoa(e.Code()) + ": " + e.Message()
}

func newError(ptr *C.struct_BtcK_Error) error {
	new := &Error{ptr}
	runtime.SetFinalizer(new, func(obj *Error) {
		C.BtcK_Error_Free(obj.ptr)
	})
	return new
}
