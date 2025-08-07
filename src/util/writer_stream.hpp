// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <btck/btck.h>

#include <serialize.h>

#include <system_error>

namespace util {

class WriterStream
{
public:
  WriterStream(BtcK_WriteBytes write, void* userdata)
    : write_{write}
    , userdata_{userdata}
  {}

  void write(std::span<std::byte const> buffer)
  {
    if (write_(buffer.data(), buffer.size(), userdata_) != 0) {
      throw std::system_error(std::make_error_code(std::errc::io_error));
    }
  }

  template <typename T> auto operator<<(T const& obj) -> WriterStream&
  {
    ::Serialize(*this, obj);
    return (*this);
  }

private:
  BtcK_WriteBytes write_;
  void* userdata_;
};

}  // namespace util
