# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

from __future__ import annotations
import typing

@typing.final
class _Slice[T]:
    def __len__(self) -> int: ...
    @typing.overload
    def __getitem__(self, index: int) -> T: ...
    @typing.overload
    def __getitem__(self, index: slice) -> _Slice[T]: ...


@typing.final
class Block:
    def __init__(self, raw: bytes): ...
    hash: BlockHash
    transactions: _Slice[Transaction]


@typing.final
class BlockHash:
    def __eq__(self, other: typing.Any) -> bool: ...
    def __ne__(self, other: typing.Any) -> bool: ...
    def __bytes__(self) -> bytes: ...


@typing.final
class Chain:
    #def __init__(self): ...
    blocks: _Slice[Block]
    #def import_blocks(self, paths: typing.Iterable[str]) -> bool: ...
    #def process_block(self, block: Block) -> tuple[bool, bool]: ...


@typing.final
class ScriptPubkey:
    def __init__(self, raw: bytes): ...
    def __eq__(self, other: typing.Any) -> bool: ...
    def __ne__(self, other: typing.Any) -> bool: ...
    def __bytes__(self) -> bytes: ...


@typing.final
class Transaction:
    def __init__(self, raw: bytes): ...
    outputs: _Slice[TransactionOutput]


@typing.final
class TransactionOutput:
    def __init__(self,  amount: int, script_pubkey: ScriptPubkey): ...
    amount: int
    script_pubkey: ScriptPubkey
