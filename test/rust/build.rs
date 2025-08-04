// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

fn main() {
    if let Ok(libdir) = std::env::var("BTCK_LIBDIR") {
        println!("cargo:rustc-link-search=native={}", libdir);
    }
}
