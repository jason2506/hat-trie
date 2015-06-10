/************************************************
 *  hat_trie.cpp
 *  HAT-Trie
 *
 *  Copyright (c) 2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#include <gtest/gtest.h>

#include "hat_trie/hat_trie.hpp"

using hat_trie::hat_trie;

TEST(HatTrieTest, CreateEmptyTrie)
{
    hat_trie<uint32_t, 1, 3> trie;
    EXPECT_TRUE(trie.empty());
    EXPECT_EQ(0, trie.size());
}
