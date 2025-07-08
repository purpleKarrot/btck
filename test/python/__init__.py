# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

import os

if os.name == "nt":
    _dll_dirs = [
        os.add_dll_directory(path_dir)
        for path_dir in os.environ.get("PATH", "").split(os.pathsep)
        if path_dir and os.path.isdir(path_dir)
    ]
