// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

import java.nio.ByteBuffer;
import java.util.Iterator;
import java.util.NoSuchElementException;

public class Block implements AutoCloseable, Iterable<Transaction> {

    public Block(ByteBuffer raw) {
        this.impl = construct(raw);
    }

    public native int size();

    public native Transaction get(int idx);

    @Override
    public Iterator<Transaction> iterator() {
        return new Iterator<Transaction>() {
            private int index = 0;
            private final int size = size();

            @Override
            public boolean hasNext() {
                return index < size;
            }

            @Override
            public Transaction next() {
                if (!hasNext()) throw new NoSuchElementException();
                return get(index++);
            }
        };
    }

    public native ByteBuffer asByteBuffer();

    @Override
    public native void close();

    private static native long construct(ByteBuffer raw);

    private long impl;
}
