// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

public class ScriptPubkey implements AutoCloseable {

    public ScriptPubkey(byte[] raw) {
        this.nativeHandle = construct(raw);
    }

    public native boolean equal(ScriptPubkey other);

    @Override
    public native void close();

    private static native long construct(byte[] raw);

    private long nativeHandle;
}
