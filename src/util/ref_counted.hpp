// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <atomic>

namespace util {

template <typename T>
class RefCounted
{
public:
  template <typename... Args>
  static auto New(Args&&... args) -> T*
  {
    try {
      return new T{std::forward<Args>(args)...};
    }
    catch (...) {
      // TODO: Add error handling here
      return nullptr;
    }
  }

  auto Retain() -> T*
  {
    this->refcount.fetch_add(1, std::memory_order_relaxed);
    return static_cast<T*>(this);
  }

  void Release()
  {
    if (this->refcount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
      delete static_cast<T*>(this);
    }
  }

private:
  RefCounted() = default;
  friend T;

  std::atomic<std::size_t> refcount = 1;
};

}  // namespace util
