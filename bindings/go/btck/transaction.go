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

type Transaction struct {
	ptr *C.struct_BtcK_Transaction
}

func NewTransaction(raw []byte) *Transaction {
	ptr := C.BtcK_Transaction_New(ptrToSlice(raw), C.size_t(len(raw)))
	return newTransactionFinalized(ptr)
}

func newTransactionFinalized(ptr *C.struct_BtcK_Transaction) *Transaction {
	t := &Transaction{ptr}
	runtime.SetFinalizer(t, func(obj *Transaction) {
		C.BtcK_Transaction_Release(obj.ptr)
	})
	return t
}

func (t *Transaction) Len() int {
	return int(C.BtcK_Transaction_GetSize(t.ptr))
}

func (t *Transaction) At(idx int) *TransactionOutput {
	ptr := C.BtcK_Transaction_At(t.ptr, C.size_t(idx))
	return newTransactionOutputFinalized(ptr)
}

func (t *Transaction) Bytes() []byte {
	var length C.size_t
	ptr := C.BtcK_Transaction_AsBytes(t.ptr, &length)
	if ptr == nil || length == 0 {
		return nil
	}
	return C.GoBytes(ptr, C.int(length))
}
