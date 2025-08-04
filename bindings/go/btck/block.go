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

type BlockHash [32]byte

type Block struct {
	ptr *C.struct_BtcK_Block
}

func NewBlock(raw []byte) (*Block, error) {
	var err *C.struct_BtcK_Error
	ptr := C.BtcK_Block_New(ptrToSlice(raw), C.size_t(len(raw)), &err)
	if err != nil {
		return nil, newError(err)
	}
	return newBlockFinalized(ptr), nil
}

func newBlockFinalized(ptr *C.struct_BtcK_Block) *Block {
	b := &Block{ptr}
	runtime.SetFinalizer(b, func(obj *Block) {
		C.BtcK_Block_Free(obj.ptr)
	})
	return b
}

func (b *Block) Hash() BlockHash {
	var hash C.struct_BtcK_BlockHash
	C.BtcK_Block_GetHash(b.ptr, &hash)
	var out BlockHash
	for i := range 32 {
		out[i] = byte(hash.data[i])
	}
	return out
}

func (b *Block) CountTransactions() int {
	return int(C.BtcK_Block_CountTransactions(b.ptr))
}

func (b *Block) GetTransaction(idx int) *Transaction {
	return &Transaction{C.BtcK_Block_GetTransaction(b.ptr, C.size_t(idx))}
}

func (b *Block) Bytes() []byte {
	var length C.size_t
	ptr := C.BtcK_Block_AsBytes(b.ptr, &length)
	if ptr == nil || length == 0 {
		return nil
	}
	return C.GoBytes(ptr, C.int(length))
}
