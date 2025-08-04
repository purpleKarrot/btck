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
import "runtime"

type ScriptPubkey struct {
	ptr *C.struct_BtcK_ScriptPubkey
}

func NewScriptPubkey(raw []byte) (*ScriptPubkey, error) {
	var err *C.struct_BtcK_Error
	ptr := C.BtcK_ScriptPubkey_New(ptrToSlice(raw), C.size_t(len(raw)), &err)
	if err != nil {
		return nil, newError(err)
	}
	return newScriptPubkeyFinalized(ptr), nil
}

func newScriptPubkeyFinalized(ptr *C.struct_BtcK_ScriptPubkey) *ScriptPubkey {
	s := &ScriptPubkey{ptr}
	runtime.SetFinalizer(s, func(obj *ScriptPubkey) {
		C.BtcK_ScriptPubkey_Release(obj.ptr)
	})
	return s
}

func (s *ScriptPubkey) Equal(other *ScriptPubkey) bool {
	return C.BtcK_ScriptPubkey_Equal(s.ptr, other.ptr) != 0
}

func (s *ScriptPubkey) Bytes() []byte {
	var length C.size_t
	ptr := C.BtcK_ScriptPubkey_AsBytes(s.ptr, &length)
	if ptr == nil || length == 0 {
		return nil
	}
	return C.GoBytes(ptr, C.int(length))
}
