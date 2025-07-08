// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

public class TransactionOutput implements AutoCloseable {

    public TransactionOutput(long amount, ScriptPubkey scriptPubkey) {
        this.impl = construct(amount, scriptPubkey);
    }

    public native long getAmount();

    public native ScriptPubkey getScriptPubkey();

    @Override
    public native void close();

    private static native long construct(
        long amount,
        ScriptPubkey scriptPubkey
    );

    private long impl;
}
