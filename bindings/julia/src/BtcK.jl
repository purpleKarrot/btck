# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

module BtcK

using Base: unsafe_convert

# Library name (adjust if needed)
const libbtck = "libbtck"

# Opaque C types
mutable struct ScriptPubkey
    ptr::Ptr{Cvoid}
end

mutable struct TransactionOutput
    ptr::Ptr{Cvoid}
end

mutable struct Transaction
    ptr::Ptr{Cvoid}
end

mutable struct Block
    ptr::Ptr{Cvoid}
end

mutable struct Chain
    ptr::Ptr{Cvoid}
end

struct BlockHash
    data::NTuple{32, UInt8}
end

# =========================
# ScriptPubkey
# =========================

function ScriptPubkey(raw::Vector{UInt8})
    ptr = ccall((:btck_ScriptPubkey_New, libbtck), Ptr{Cvoid}, (Ptr{Cvoid}, Csize_t), pointer(raw), length(raw))
    obj = ScriptPubkey(ptr)
    finalizer(release!, obj)
    return obj
end

function retain(obj::ScriptPubkey)
    ptr = ccall((:btck_ScriptPubkey_Retain, libbtck), Ptr{Cvoid}, (Ptr{Cvoid},), obj.ptr)
    ScriptPubkey(ptr)
end

function release!(obj::ScriptPubkey)
    if obj.ptr != C_NULL
        ccall((:btck_ScriptPubkey_Free, libbtck), Cvoid, (Ptr{Cvoid},), obj.ptr)
        obj.ptr = C_NULL
    end
    nothing
end

function isequal(a::ScriptPubkey, b::ScriptPubkey)
    ccall((:btck_ScriptPubkey_Equal, libbtck), Cint, (Ptr{Cvoid}, Ptr{Cvoid}), a.ptr, b.ptr) != 0
end

function asbytes(obj::ScriptPubkey)
    len = Ref{Csize_t}()
    ptr = ccall((:btck_ScriptPubkey_AsBytes, libbtck), Ptr{UInt8}, (Ptr{Cvoid}, Ref{Csize_t}), obj.ptr, len)
    unsafe_wrap(Vector{UInt8}, ptr, len[])
end

# =========================
# TransactionOutput
# =========================

function TransactionOutput(amount::Int64, script_pubkey::ScriptPubkey)
    ptr = ccall((:btck_TransactionOutput_New, libbtck), Ptr{Cvoid},
        (Int64, Ptr{Cvoid}), amount, script_pubkey.ptr)
    obj = TransactionOutput(ptr)
    finalizer(release!, obj)
    return obj
end

function retain(obj::TransactionOutput)
    ptr = ccall((:btck_TransactionOutput_Retain, libbtck), Ptr{Cvoid}, (Ptr{Cvoid},), obj.ptr)
    TransactionOutput(ptr)
end

function release!(obj::TransactionOutput)
    if obj.ptr != C_NULL
        ccall((:btck_TransactionOutput_Free, libbtck), Cvoid, (Ptr{Cvoid},), obj.ptr)
        obj.ptr = C_NULL
    end
    nothing
end

function getamount(obj::TransactionOutput)
    ccall((:btck_TransactionOutput_GetAmount, libbtck), Int64, (Ptr{Cvoid},), obj.ptr)
end

function getscriptpubkey(obj::TransactionOutput)
    ptr = ccall((:btck_TransactionOutput_GetScriptPubkey, libbtck), Ptr{Cvoid}, (Ptr{Cvoid},), obj.ptr)
    ScriptPubkey(ptr)
end

# =========================
# Transaction
# =========================

function Transaction(raw::Vector{UInt8})
    ptr = ccall((:btck_Transaction_New, libbtck), Ptr{Cvoid}, (Ptr{Cvoid}, Csize_t), pointer(raw), length(raw))
    obj = Transaction(ptr)
    finalizer(release!, obj)
    return obj
end

function retain(obj::Transaction)
    ptr = ccall((:btck_Transaction_Retain, libbtck), Ptr{Cvoid}, (Ptr{Cvoid},), obj.ptr)
    Transaction(ptr)
end

function release!(obj::Transaction)
    if obj.ptr != C_NULL
        ccall((:btck_Transaction_Free, libbtck), Cvoid, (Ptr{Cvoid},), obj.ptr)
        obj.ptr = C_NULL
    end
    nothing
end

function size(obj::Transaction)
    ccall((:btck_Transaction_GetSize, libbtck), Csize_t, (Ptr{Cvoid},), obj.ptr)
end

function at(obj::Transaction, idx::Integer)
    ptr = ccall((:btck_Transaction_At, libbtck), Ptr{Cvoid}, (Ptr{Cvoid}, Csize_t), obj.ptr, idx)
    TransactionOutput(ptr)
end

function asbytes(obj::Transaction)
    len = Ref{Csize_t}()
    ptr = ccall((:btck_Transaction_AsBytes, libbtck), Ptr{UInt8}, (Ptr{Cvoid}, Ref{Csize_t}), obj.ptr, len)
    unsafe_wrap(Vector{UInt8}, ptr, len[])
end

function tostring(obj::Transaction)
    len = Ref{Csize_t}()
    ptr = ccall((:btck_Transaction_ToString, libbtck), Ptr{UInt8}, (Ptr{Cvoid}, Ref{Csize_t}), obj.ptr, len)
    String(unsafe_wrap(Vector{UInt8}, ptr, len[]))
end

# =========================
# BlockHash
# =========================

function BlockHash(raw::Vector{UInt8})
    if length(raw) != 32
        throw(ArgumentError("BlockHash must be 32 bytes"))
    end
    buf = zeros(UInt8, 32)
    unsafe_copyto!(pointer(buf), pointer(raw), 32)
    ccall((:btck_BlockHash_Init, libbtck), Cvoid, (Ptr{UInt8}, Ptr{Cvoid}, Csize_t), pointer(buf), pointer(raw), 32)
    BlockHash(Tuple(buf))
end

# =========================
# Block
# =========================

function Block(raw::Vector{UInt8})
    ptr = ccall((:btck_Block_New, libbtck), Ptr{Cvoid}, (Ptr{Cvoid}, Csize_t), pointer(raw), length(raw))
    obj = Block(ptr)
    finalizer(release!, obj)
    return obj
end

function retain(obj::Block)
    ptr = ccall((:btck_Block_Retain, libbtck), Ptr{Cvoid}, (Ptr{Cvoid},), obj.ptr)
    Block(ptr)
end

function release!(obj::Block)
    if obj.ptr != C_NULL
        ccall((:btck_Block_Free, libbtck), Cvoid, (Ptr{Cvoid},), obj.ptr)
        obj.ptr = C_NULL
    end
    nothing
end

function gethash(obj::Block)
    buf = zeros(UInt8, 32)
    ccall((:btck_Block_GetHash, libbtck), Cvoid, (Ptr{Cvoid}, Ptr{UInt8}), obj.ptr, pointer(buf))
    BlockHash(Tuple(buf))
end

function size(obj::Block)
    ccall((:btck_Block_GetSize, libbtck), Csize_t, (Ptr{Cvoid},), obj.ptr)
end

function at(obj::Block, idx::Integer)
    ptr = ccall((:btck_Block_At, libbtck), Ptr{Cvoid}, (Ptr{Cvoid}, Csize_t), obj.ptr, idx)
    Transaction(ptr)
end

function asbytes(obj::Block)
    len = Ref{Csize_t}()
    ptr = ccall((:btck_Block_AsBytes, libbtck), Ptr{UInt8}, (Ptr{Cvoid}, Ref{Csize_t}), obj.ptr, len)
    unsafe_wrap(Vector{UInt8}, ptr, len[])
end

# =========================
# Chain
# =========================

function Chain()
    ptr = ccall((:btck_Chain_New, libbtck), Ptr{Cvoid}, ())
    obj = Chain(ptr)
    finalizer(release!, obj)
    return obj
end

function retain(obj::Chain)
    ptr = ccall((:btck_Chain_Retain, libbtck), Ptr{Cvoid}, (Ptr{Cvoid},), obj.ptr)
    Chain(ptr)
end

function release!(obj::Chain)
    if obj.ptr != C_NULL
        ccall((:btck_Chain_Free, libbtck), Cvoid, (Ptr{Cvoid},), obj.ptr)
        obj.ptr = C_NULL
    end
    nothing
end

function size(obj::Chain)
    ccall((:btck_Chain_GetSize, libbtck), Csize_t, (Ptr{Cvoid},), obj.ptr)
end

function at(obj::Chain, idx::Integer)
    ptr = ccall((:btck_Chain_At, libbtck), Ptr{Cvoid}, (Ptr{Cvoid}, Csize_t), obj.ptr, idx)
    Block(ptr)
end

function find(obj::Chain, block_hash::BlockHash)
    buf = collect(block_hash.data)
    ccall((:btck_Chain_Find, libbtck), Clong, (Ptr{Cvoid}, Ptr{UInt8}), obj.ptr, pointer(buf))
end

end # module Btck
