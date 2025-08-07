// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <btck/btck.hpp>
#include <btck/btck_error.hpp>
#include <cstring>
#include <exception>
#include <new>
#include <stdexcept>
#include <system_error>

namespace {

void throw_domain(
  btck::detail::error const& err, std::error_category const& domain)
{
  if (std::strcmp(err.domain(), domain.name()) == 0) {
    throw std::system_error(err.code(), domain);
  }
}

}  // namespace

void btck::detail::translate_error(error const& err)
{
  using namespace std::literals;
  if (err.domain() == "Memory"sv) {
    throw std::bad_alloc();
  }
  throw_domain(err, std::generic_category());  // TODO: Is this portable?
  throw_domain(err, std::system_category());   // TODO: Is this portable?
  throw_domain(err, btck::verification_error_category());
  throw std::runtime_error{err.message()};
}

auto btck::detail::to_bytes_(void const* obj, to_bytes_fn writefn)
  -> std::vector<std::byte>
{
  std::vector<std::byte> bytes;

  struct closure_t {
    std::vector<std::byte>* bytes;
    std::exception_ptr exception;
  };

  constexpr auto const write = +[](void const* buf, size_t len, void* user) {
    auto& closure = *reinterpret_cast<closure_t*>(user);
    try {
      auto const* first = static_cast<std::byte const*>(buf);
      auto const* last = first + len;
      closure.bytes->insert(closure.bytes->end(), first, last);
      return 0;
    }
    catch (...) {
      closure.exception = std::current_exception();
      return -1;
    }
  };

  auto closure = closure_t{.bytes = &bytes};
  if (writefn(obj, write, &closure) != 0) {
    std::rethrow_exception(closure.exception);
  }

  return bytes;
}

auto btck::detail::to_string_(void const* obj, to_string_fn printfn)
  -> std::string
{
  auto const len = printfn(obj, nullptr, 0);
  if (len < 0) {
    throw std::runtime_error("to_string failed");
  }
  auto buf = std::string(static_cast<std::string::size_type>(len), '\0');
  printfn(obj, buf.data(), len + 1);
  return buf;
}
