// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <string>

#include <btck/btck.hpp>
#include <gtest/gtest.h>

TEST(Verify, Flags)
{
  EXPECT_EQ(to_string(btck::script_verify::all), "ALL");
  EXPECT_EQ(to_string(btck::script_verify::none), "NONE");
  EXPECT_EQ(
    to_string(btck::script_verify::witness | btck::script_verify::taproot),
    "WITNESS | TAPROOT"
  );

  EXPECT_ANY_THROW(to_string(static_cast<btck::script_verify>(2)));

  EXPECT_EQ(
    btck::script_verify::all,
    btck::script_verify::p2sh | btck::script_verify::dersig |
      btck::script_verify::nulldummy |
      btck::script_verify::checklocktimeverify |
      btck::script_verify::checksequenceverify | btck::script_verify::witness |
      btck::script_verify::taproot
  );
}
