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

type TransactionOutput struct {
	ptr *C.struct_BtcK_TransactionOutput
}

func NewTransactionOutput(amount int64, scriptPubkey *ScriptPubkey) *TransactionOutput {
	ptr := C.BtcK_TransactionOutput_New(C.int64_t(amount), scriptPubkey.ptr)
	return newTransactionOutputFinalized(ptr)
}

func newTransactionOutputFinalized(ptr *C.struct_BtcK_TransactionOutput) *TransactionOutput {
	t := &TransactionOutput{ptr}
	runtime.SetFinalizer(t, func(obj *TransactionOutput) {
		C.BtcK_TransactionOutput_Release(obj.ptr)
	})
	return t
}

func (t *TransactionOutput) Amount() int64 {
	return int64(C.BtcK_TransactionOutput_GetAmount(t.ptr))
}

func (t *TransactionOutput) ScriptPubkey() *ScriptPubkey {
	return &ScriptPubkey{C.BtcK_TransactionOutput_GetScriptPubkey(t.ptr)}
}
