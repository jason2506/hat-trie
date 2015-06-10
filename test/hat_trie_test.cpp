/************************************************
 *  hat_trie.cpp
 *  HAT-Trie
 *
 *  Copyright (c) 2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#include <string>

#include <gtest/gtest.h>

#include "hat_trie/hat_trie.hpp"

using std::string;
using hat_trie::hat_trie;

void check_key_and_value(hat_trie<uint32_t, 1, 3>::iterator const &it,
                         string const &expect_key, uint32_t expect_val)
{
    string key(it->get_key().begin(), it->get_key().end());
    EXPECT_EQ(expect_key, key);
    EXPECT_EQ(expect_val, it->get_value());
}

TEST(HatTrieTest, CreateEmptyTrie)
{
    hat_trie<uint32_t, 1, 3> trie;
    EXPECT_TRUE(trie.empty());
    EXPECT_EQ(0, trie.size());
}

TEST(HatTrieTest, InsertKeyAndValue)
{
    hat_trie<uint32_t, 1, 3> trie;

    auto pair1 = trie.insert("hello", 3);
    check_key_and_value(pair1.first, "hello", 3);
    EXPECT_TRUE(pair1.second);
    EXPECT_EQ(1, trie.size());
    EXPECT_FALSE(trie.empty());

    auto pair2 = trie.insert("hi", 5);
    check_key_and_value(pair2.first, "hi", 5);
    EXPECT_TRUE(pair2.second);
    EXPECT_EQ(2, trie.size());
}
