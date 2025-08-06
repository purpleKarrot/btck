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

func NewTransaction(raw []byte) (*Transaction, error) {
	var err *C.struct_BtcK_Error
	ptr := C.BtcK_Transaction_New(ptrToSlice(raw), C.size_t(len(raw)), &err)
	if err != nil {
		return nil, newError(err)
	}
	return newTransactionFinalized(ptr), nil
}

func newTransactionFinalized(ptr *C.struct_BtcK_Transaction) *Transaction {
	t := &Transaction{ptr}
	runtime.SetFinalizer(t, func(obj *Transaction) {
		C.BtcK_Transaction_Free(obj.ptr)
	})
	return t
}

func (t *Transaction) CountOutputs() int {
	return int(C.BtcK_Transaction_CountOutputs(t.ptr))
}

func (t *Transaction) GetOutput(idx int) (*TransactionOutput, error) {
	var err *C.struct_BtcK_Error
	ptr := C.BtcK_Transaction_GetOutput(t.ptr, C.size_t(idx), &err)
	if err != nil {
		return nil, newError(err)
	}
	return newTransactionOutputFinalized(ptr), nil
}

func (t *Transaction) Bytes() []byte {
	var length C.size_t
	ptr := C.BtcK_Transaction_AsBytes(t.ptr, &length)
	if ptr == nil || length == 0 {
		return nil
	}
	return C.GoBytes(ptr, C.int(length))
}
