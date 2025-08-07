// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <btck/btck.h>
#include <primitives/block.h>

namespace api {

template <typename T> struct c_to_cpp;
template <typename T> struct cpp_to_c;

template <typename T>
concept c_type = requires { typename c_to_cpp<T>::type; };

template <typename T>
concept cpp_type = requires { typename cpp_to_c<T>::type; };

template <c_type T> using cpp_type_t = c_to_cpp<T>::type;
template <cpp_type T> using c_type_t = cpp_to_c<T>::type;

}  // namespace api

#define UTIL_TYPE_PAIR(C, CXX)                                                 \
  template <> struct api::c_to_cpp<C> {                                        \
    using type = CXX;                                                          \
  };                                                                           \
  template <> struct api::cpp_to_c<CXX> {                                      \
    using type = C;                                                            \
  }

UTIL_TYPE_PAIR(BtcK_Block, CBlock);
UTIL_TYPE_PAIR(BtcK_Transaction, CTransactionRef);
UTIL_TYPE_PAIR(BtcK_TransactionOutput, CTxOut);
UTIL_TYPE_PAIR(BtcK_ScriptPubkey, CScript);
