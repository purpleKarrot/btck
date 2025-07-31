// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <gtest/gtest.h>

#include <btck/btck.hpp>
#include <string>

TEST(Verify, Flags)
{
  EXPECT_EQ(to_string(btck::verification_flags::all), "ALL");
  EXPECT_EQ(to_string(btck::verification_flags::none), "NONE");
  EXPECT_EQ(
    to_string(
      btck::verification_flags::witness | btck::verification_flags::taproot),
    "WITNESS | TAPROOT");

  EXPECT_ANY_THROW(to_string(static_cast<btck::verification_flags>(2)));

  EXPECT_EQ(
    btck::verification_flags::all,
    btck::verification_flags::p2sh | btck::verification_flags::dersig |
      btck::verification_flags::nulldummy |
      btck::verification_flags::checklocktimeverify |
      btck::verification_flags::checksequenceverify |
      btck::verification_flags::witness | btck::verification_flags::taproot);
}
