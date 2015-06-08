/************************************************
 *  string_hashmap_test.cpp
 *  HAT-Trie
 *
 *  Copyright (c) 2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#include <gtest/gtest.h>

#include "hat_trie/string_hashmap.hpp"

using std::string;
using hat_trie::impl::string_hashmap;

void construct_string_hashmap(string_hashmap<uint32_t, 3> &shm)
{
    shm.insert("cat", 1);
    shm.insert("elephant", 3);
    shm.insert("rabbit", 5);
    shm.insert("bird", 7);
    shm.insert("horse", 9);
}

void check_key_and_value(string_hashmap<uint32_t, 3>::iterator const &it,
                         string const &expect_key, uint32_t expect_val)
{
    string key(it->get_key().begin(), it->get_key().end());
    EXPECT_EQ(expect_key, key);
    EXPECT_EQ(expect_val, it->get_value());
}

TEST(StringHashmap, CreateEmptyHashmap)
{
    string_hashmap<uint32_t, 3> shm;
    EXPECT_TRUE(shm.empty());
    EXPECT_EQ(0, shm.size());
    EXPECT_EQ(3, shm.bucket_count());
}

TEST(StringHashmap, InsertKeyAndValue)
{
    string_hashmap<uint32_t, 3> shm;

    auto pair1 = shm.insert("hello", 3);
    check_key_and_value(pair1.first, "hello", 3);
    EXPECT_TRUE(pair1.second);
    EXPECT_EQ(1, shm.size());
    EXPECT_FALSE(shm.empty());

    auto pair2 = shm.insert("hi", 5);
    check_key_and_value(pair2.first, "hi", 5);
    EXPECT_TRUE(pair2.second);
    EXPECT_EQ(2, shm.size());
}

TEST(StringHashmap, CheckBucketSize)
{
    string_hashmap<uint32_t, 3> shm;
    construct_string_hashmap(shm);

    size_t expected_bucket_size[3] = {0, 0, 0};
    expected_bucket_size[shm.bucket("cat")]++;
    expected_bucket_size[shm.bucket("elephant")]++;
    expected_bucket_size[shm.bucket("rabbit")]++;
    expected_bucket_size[shm.bucket("bird")]++;
    expected_bucket_size[shm.bucket("horse")]++;

    EXPECT_EQ(expected_bucket_size[0], shm.bucket_size(0));
    EXPECT_EQ(expected_bucket_size[1], shm.bucket_size(1));
    EXPECT_EQ(expected_bucket_size[2], shm.bucket_size(2));
}
