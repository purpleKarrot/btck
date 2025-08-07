// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "error.hpp"
#include "type_mapping.hpp"

namespace api {

template <cpp_type T> [[nodiscard]] auto ref(T& self) -> c_type_t<T>*
{
  return reinterpret_cast<c_type_t<T>*>(&self);
}

template <cpp_type T>
[[nodiscard]] auto ref(T const& self) -> c_type_t<T> const*
{
  return reinterpret_cast<c_type_t<T> const*>(&self);
}

template <c_type T> [[nodiscard]] auto get(T* self) -> cpp_type_t<T>&
{
  return *reinterpret_cast<cpp_type_t<T>*>(self);
}

template <c_type T>
[[nodiscard]] auto get(T const* self) -> cpp_type_t<T> const&
{
  return *reinterpret_cast<cpp_type_t<T> const*>(self);
}

template <cpp_type T, typename... Args>
[[nodiscard]] auto create(Args&&... args) -> c_type_t<T>*
{
  return reinterpret_cast<c_type_t<T>*>(new T(std::forward<Args>(args)...));
}

template <c_type T>
[[nodiscard]] auto copy(T const* self, BtcK_Error** err) -> T*
{
  return util::WrapFn(
    err, [self] { return api::create<cpp_type_t<T>>(get(self)); });
}

template <c_type T> void free(T* self)
{
  delete reinterpret_cast<cpp_type_t<T>*>(self);
}

}  // namespace api
