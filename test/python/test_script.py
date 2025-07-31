# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

import btck
import pytest

def test_script_verify():
    assert btck.VerificationFlags.ALL|btck.VerificationFlags.NONE == btck.VerificationFlags.ALL


def test_script1():
    # a random old-style transaction from the blockchain
    assert btck.verify_script(
        script_pubkey = btck.ScriptPubkey(
            bytes.fromhex("76a9144bfbaf6afb76cc5771bc6404810d1cc041a6933988ac"),
        ),
        amount = 0,
        tx_to = btck.Transaction(bytes.fromhex("02000000013f7cebd65c27431a90bba7f796914fe8cc2ddfc3f2cbd6f7e5f2fc854534da95000000006b483045022100de1ac3bcdfb0332207c4a91f3832bd2c2915840165f876ab47c5f8996b971c3602201c6c053d750fadde599e6f5c4e1963df0f01fc0d97815e8157e3d59fe09ca30d012103699b464d1d8bc9e47d4fb1cdaa89a1c5783d68363c4dbc4b524ed3d857148617feffffff02836d3c01000000001976a914fc25d6d5c94003bf5b0c7b640a248e2c637fcfb088ac7ada8202000000001976a914fbed3d9b11183209a57999d54d59f67c019e756c88ac6acb0700")),
        flags = btck.VerificationFlags.ALL & ~btck.VerificationFlags.TAPROOT
    )


def test_script2():
    # a random segwit transaction from the blockchain using P2SH
    assert btck.verify_script(
        script_pubkey = btck.ScriptPubkey(
            bytes.fromhex("a91434c06f8c87e355e123bdc6dda4ffabc64b6989ef87"),
        ),
        amount = 1900000,
        tx_to = btck.Transaction(bytes.fromhex("01000000000101d9fd94d0ff0026d307c994d0003180a5f248146efb6371d040c5973f5f66d9df0400000017160014b31b31a6cb654cfab3c50567bcf124f48a0beaecffffffff012cbd1c000000000017a914233b74bf0823fa58bbbd26dfc3bb4ae715547167870247304402206f60569cac136c114a58aedd80f6fa1c51b49093e7af883e605c212bdafcd8d202200e91a55f408a021ad2631bc29a67bd6915b2d7e9ef0265627eabd7f7234455f6012103e7e802f50344303c76d12c089c8724c1b230e3b745693bbe16aad536293d15e300000000")),
        flags = btck.VerificationFlags.ALL & ~btck.VerificationFlags.TAPROOT
    )


def test_script3():
    # a random segwit transaction from the blockchain using native segwit
    assert btck.verify_script(
        script_pubkey = btck.ScriptPubkey(
            bytes.fromhex("0020701a8d401c84fb13e6baf169d59684e17abd9fa216c8cc5b9fc63d622ff8c58d"),
        ),
        amount = 18393430,
        tx_to = btck.Transaction(bytes.fromhex("010000000001011f97548fbbe7a0db7588a66e18d803d0089315aa7d4cc28360b6ec50ef36718a0100000000ffffffff02df1776000000000017a9146c002a686959067f4866b8fb493ad7970290ab728757d29f0000000000220020701a8d401c84fb13e6baf169d59684e17abd9fa216c8cc5b9fc63d622ff8c58d04004730440220565d170eed95ff95027a69b313758450ba84a01224e1f7f130dda46e94d13f8602207bdd20e307f062594022f12ed5017bbf4a055a06aea91c10110a0e3bb23117fc014730440220647d2dc5b15f60bc37dc42618a370b2a1490293f9e5c8464f53ec4fe1dfe067302203598773895b4b16d37485cbe21b337f4e4b650739880098c592553add7dd4355016952210375e00eb72e29da82b89367947f29ef34afb75e8654f6ea368e0acdfd92976b7c2103a1b26313f430c4b15bb1fdce663207659d8cac749a0e53d70eff01874496feff2103c96d495bfdd5ba4145e3e046fee45e84a8a48ad05bd8dbb395c011a32cf9f88053ae00000000")),
        flags = btck.VerificationFlags.ALL & ~btck.VerificationFlags.TAPROOT
    )


def test_script4():
    # a random old-style transaction from the blockchain - WITH WRONG SIGNATURE for the address
    assert not btck.verify_script(
        script_pubkey = btck.ScriptPubkey(
            bytes.fromhex("76a9144bfbaf6afb76cc5771bc6404810d1cc041a6933988ff"),
        ),
        amount = 0,
        tx_to = btck.Transaction(bytes.fromhex("02000000013f7cebd65c27431a90bba7f796914fe8cc2ddfc3f2cbd6f7e5f2fc854534da95000000006b483045022100de1ac3bcdfb0332207c4a91f3832bd2c2915840165f876ab47c5f8996b971c3602201c6c053d750fadde599e6f5c4e1963df0f01fc0d97815e8157e3d59fe09ca30d012103699b464d1d8bc9e47d4fb1cdaa89a1c5783d68363c4dbc4b524ed3d857148617feffffff02836d3c01000000001976a914fc25d6d5c94003bf5b0c7b640a248e2c637fcfb088ac7ada8202000000001976a914fbed3d9b11183209a57999d54d59f67c019e756c88ac6acb0700")),
        flags = btck.VerificationFlags.ALL & ~btck.VerificationFlags.TAPROOT
    )


def test_script5():
    # a random segwit transaction from the blockchain using P2SH - WITH WRONG AMOUNT
    assert not btck.verify_script(
        script_pubkey = btck.ScriptPubkey(
            bytes.fromhex("a91434c06f8c87e355e123bdc6dda4ffabc64b6989ef87"),
        ),
        amount = 900000,
        tx_to = btck.Transaction(bytes.fromhex("01000000000101d9fd94d0ff0026d307c994d0003180a5f248146efb6371d040c5973f5f66d9df0400000017160014b31b31a6cb654cfab3c50567bcf124f48a0beaecffffffff012cbd1c000000000017a914233b74bf0823fa58bbbd26dfc3bb4ae715547167870247304402206f60569cac136c114a58aedd80f6fa1c51b49093e7af883e605c212bdafcd8d202200e91a55f408a021ad2631bc29a67bd6915b2d7e9ef0265627eabd7f7234455f6012103e7e802f50344303c76d12c089c8724c1b230e3b745693bbe16aad536293d15e300000000")),
        flags = btck.VerificationFlags.ALL & ~btck.VerificationFlags.TAPROOT
    )


def test_script6():
    # a random segwit transaction from the blockchain using native segwit - WITH WRONG SEGWIT
    assert not btck.verify_script(
        script_pubkey = btck.ScriptPubkey(
            bytes.fromhex("0020701a8d401c84fb13e6baf169d59684e17abd9fa216c8cc5b9fc63d622ff8c58f"),
        ),
        amount = 18393430,
        tx_to = btck.Transaction(bytes.fromhex("010000000001011f97548fbbe7a0db7588a66e18d803d0089315aa7d4cc28360b6ec50ef36718a0100000000ffffffff02df1776000000000017a9146c002a686959067f4866b8fb493ad7970290ab728757d29f0000000000220020701a8d401c84fb13e6baf169d59684e17abd9fa216c8cc5b9fc63d622ff8c58d04004730440220565d170eed95ff95027a69b313758450ba84a01224e1f7f130dda46e94d13f8602207bdd20e307f062594022f12ed5017bbf4a055a06aea91c10110a0e3bb23117fc014730440220647d2dc5b15f60bc37dc42618a370b2a1490293f9e5c8464f53ec4fe1dfe067302203598773895b4b16d37485cbe21b337f4e4b650739880098c592553add7dd4355016952210375e00eb72e29da82b89367947f29ef34afb75e8654f6ea368e0acdfd92976b7c2103a1b26313f430c4b15bb1fdce663207659d8cac749a0e53d70eff01874496feff2103c96d495bfdd5ba4145e3e046fee45e84a8a48ad05bd8dbb395c011a32cf9f88053ae00000000")),
        flags = btck.VerificationFlags.ALL & ~btck.VerificationFlags.TAPROOT
    )


def test_script7():
    # `TAPROOT` without providing `spent_outputs`
    with pytest.raises(btck.VerificationError) as err:
        btck.verify_script(
            script_pubkey = btck.ScriptPubkey(
                bytes.fromhex("76a9144bfbaf6afb76cc5771bc6404810d1cc041a6933988ac"),
            ),
            amount = 0,
            tx_to = btck.Transaction(bytes.fromhex("02000000013f7cebd65c27431a90bba7f796914fe8cc2ddfc3f2cbd6f7e5f2fc854534da95000000006b483045022100de1ac3bcdfb0332207c4a91f3832bd2c2915840165f876ab47c5f8996b971c3602201c6c053d750fadde599e6f5c4e1963df0f01fc0d97815e8157e3d59fe09ca30d012103699b464d1d8bc9e47d4fb1cdaa89a1c5783d68363c4dbc4b524ed3d857148617feffffff02836d3c01000000001976a914fc25d6d5c94003bf5b0c7b640a248e2c637fcfb088ac7ada8202000000001976a914fbed3d9b11183209a57999d54d59f67c019e756c88ac6acb0700")),
            flags = btck.VerificationFlags.ALL
        )

    assert str(err.value) == "The taproot flag was set, so valid spent_outputs have to be provided."
