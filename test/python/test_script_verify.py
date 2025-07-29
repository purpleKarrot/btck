# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

import btck

def test_script_verify():
    assert btck.ScriptVerify.ALL|btck.ScriptVerify.NONE == btck.ScriptVerify.ALL
