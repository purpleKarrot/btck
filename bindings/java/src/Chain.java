// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

import java.util.Iterator;
import java.util.NoSuchElementException;

public class Chain implements AutoCloseable, Iterable<Block> {

    public Chain() {
        this.impl = construct();
    }

    public native int size();

    public native Block get(int idx);

    @Override
    public Iterator<Block> iterator() {
        return new Iterator<Block>() {
            private int index = 0;
            private final int size = size();

            @Override
            public boolean hasNext() {
                return index < size;
            }

            @Override
            public Block next() {
                if (!hasNext()) throw new NoSuchElementException();
                return get(index++);
            }
        };
    }

    @Override
    public native void close();

    private static native long construct();

    private long impl;
}
