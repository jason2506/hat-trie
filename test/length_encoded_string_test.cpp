/************************************************
 *  length_encoded_string_test.cpp
 *  HAT-Trie
 *
 *  Copyright (c) 2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#include <algorithm>

#include <gtest/gtest.h>

#include "hat_trie/length_encoded_string.hpp"

using std::equal;
using hat_trie::impl::prefix_size;
using hat_trie::impl::encode_string;
using hat_trie::impl::length_encoded_string;

TEST(LengthEncodedString, PrefixSize)
{
    EXPECT_EQ(1, prefix_size(0));
    EXPECT_EQ(1, prefix_size(255));

    // NOTE: Currently, we use only one byte to store the length of string, and
    //       hence we should not pass the number which is greater than 255.
    // EXPECT_EQ(2, prefix_size(256));
}

TEST(LengthEncodedString, EncodeEmptyString)
{
    char buf[1];
    encode_string(buf, "");
    EXPECT_EQ(0, buf[0]);
}

TEST(LengthEncodedString, EncodeNormalString)
{
    char buf[13];
    char const s[] = "hello, world";
    encode_string(buf, s);

    EXPECT_EQ(12, buf[0]);
    EXPECT_TRUE(equal(buf + 1, buf + 13, s));
}

TEST(LengthEncodedString, MakeEncodedStringWrapper)
{
    char buf[13];
    char const s[] = "hello, world";
    encode_string(buf, s);

    length_encoded_string les(static_cast<char const *>(buf));
    EXPECT_EQ(buf, les.data());
    EXPECT_EQ(13, les.bytes_count());
    EXPECT_EQ(1, les.prefix_size());
    EXPECT_EQ(12, les.size());
    EXPECT_EQ(12, les.end() - les.begin());
    EXPECT_TRUE(equal(les.begin(), les.end(), s));
}
