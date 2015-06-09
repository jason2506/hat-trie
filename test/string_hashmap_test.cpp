/************************************************
 *  string_hashmap_test.cpp
 *  HAT-Trie
 *
 *  Copyright (c) 2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#include <array>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include <gtest/gtest.h>

#include "hat_trie/string_hashmap.hpp"

using std::array;
using std::out_of_range;
using std::string;
using std::unordered_map;
using hat_trie::impl::string_hashmap;

void construct_string_hashmap(string_hashmap<uint32_t, 3> &shm)
{
    shm.insert("cat", 1);
    shm.insert("elephant", 3);
    shm.insert("rabbit", 5);
    shm.insert("bird", 7);
    shm.insert("horse", 9);
}

string get_key(string_hashmap<uint32_t, 3>::iterator const &it)
{
    return string(it->get_key().begin(), it->get_key().end());
}

void check_key_and_value(string_hashmap<uint32_t, 3>::iterator const &it,
                         string const &expect_key, uint32_t expect_val)
{
    EXPECT_EQ(expect_key, get_key(it));
    EXPECT_EQ(expect_val, it->get_value());
}

TEST(StringHashmap, CreateEmptyHashmap)
{
    string_hashmap<uint32_t, 3> shm;
    EXPECT_TRUE(shm.empty());
    EXPECT_EQ(0, shm.size());
    EXPECT_EQ(3, shm.bucket_count());
    EXPECT_EQ(shm.end(), shm.begin());
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

TEST(StringHashmap, TraverseHashmap)
{
    string_hashmap<uint32_t, 3> shm;
    construct_string_hashmap(shm);

    unordered_map<string, size_t> counts;
    auto it = shm.begin();
    counts[get_key(it)]++;
    counts[get_key(++it)]++;
    counts[get_key(++it)]++;
    counts[get_key(++it)]++;
    counts[get_key(++it)]++;

    EXPECT_EQ(shm.end(), ++it);
    EXPECT_EQ(1, counts["cat"]);
    EXPECT_EQ(1, counts["elephant"]);
    EXPECT_EQ(1, counts["rabbit"]);
    EXPECT_EQ(1, counts["bird"]);
    EXPECT_EQ(1, counts["horse"]);
}

TEST(StringHashmap, FindElementWithKey)
{
    string_hashmap<uint32_t, 3> shm;
    construct_string_hashmap(shm);

    auto it1 = shm.find("rabbit");
    check_key_and_value(it1, "rabbit", 5);

    auto it2 = shm.find("horse");
    check_key_and_value(it2, "horse", 9);

    auto it3 = shm.find("tiger");
    EXPECT_EQ(shm.end(), it3);
}

TEST(StringHashmap, AccessElement)
{
    string_hashmap<uint32_t, 3> shm;
    construct_string_hashmap(shm);

    EXPECT_EQ(1, shm["cat"]);
    EXPECT_EQ(7, shm["bird"]);

    EXPECT_EQ(3, shm.at("elephant"));
    EXPECT_EQ(5, shm.at("rabbit"));

    EXPECT_THROW(shm.at("tiger"), out_of_range);
}

TEST(StringHashmap, EraseElement)
{
    string_hashmap<uint32_t, 3> shm;
    construct_string_hashmap(shm);

    array<string, 5> keys;
    auto it = shm.begin();
    keys[0] = get_key(it);
    keys[1] = get_key(++it);
    keys[2] = get_key(++it);
    keys[3] = get_key(++it);
    keys[4] = get_key(++it);

    auto next_it = shm.erase(shm.begin());
    EXPECT_EQ(keys[1], get_key(next_it));

    next_it = shm.erase(++next_it);
    EXPECT_EQ(keys[3], get_key(next_it));

    next_it = shm.erase(++next_it);
    EXPECT_EQ(shm.end(), next_it);

    EXPECT_EQ(2, shm.size());

    it = shm.begin();
    EXPECT_EQ(keys[1], get_key(it));
    EXPECT_EQ(keys[3], get_key(++it));
    EXPECT_EQ(shm.end(), ++it);
}

TEST(StringHashmap, ClearHashmap)
{
    string_hashmap<uint32_t, 3> shm;
    construct_string_hashmap(shm);

    shm.clear();
    EXPECT_TRUE(shm.empty());
    EXPECT_EQ(0, shm.size());
    EXPECT_EQ(3, shm.bucket_count());
    EXPECT_EQ(shm.end(), shm.begin());
}
