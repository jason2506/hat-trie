/************************************************
 *  string_bucket_test.cpp
 *  HAT-Trie
 *
 *  Copyright (c) 2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#include <string>
#include <stdexcept>

#include <gtest/gtest.h>

#include "hat_trie/string_bucket.hpp"

using std::string;
using std::out_of_range;
using hat_trie::impl::string_bucket;

void construct_string_bucket(string_bucket<uint32_t> &sb)
{
    sb.push_back("cat", 1);
    sb.push_back("elephant", 3);
    sb.push_back("rabbit", 5);
    sb.push_back("bird", 7);
    sb.push_back("horse", 9);
}

void check_key_and_value(string_bucket<uint32_t>::iterator const &it,
                         string const &expect_key, uint32_t expect_val)
{
    string key(it->get_key().begin(), it->get_key().end());
    EXPECT_EQ(expect_key, key);
    EXPECT_EQ(expect_val, it->get_value());
}

TEST(StringBucket, CreateEmptyBucket)
{
    string_bucket<uint32_t> sb;
    EXPECT_TRUE(sb.empty());
    EXPECT_EQ(0, sb.size());
    EXPECT_EQ(0, sb.bytes_count());
}

TEST(StringBucket, InsertKeyAndValue)
{
    string_bucket<uint32_t> sb;

    auto it1 = sb.push_back("hello", 3);
    check_key_and_value(it1, "hello", 3);
    EXPECT_EQ(1, sb.size());
    EXPECT_EQ(10, sb.bytes_count()); // 1 (prefix) + 5 (key) + 4 (value) = 10 bytes
    EXPECT_FALSE(sb.empty());

    auto it2 = sb.push_back("hi", 5);
    check_key_and_value(it2, "hi", 5);
    EXPECT_EQ(2, sb.size());
    EXPECT_EQ(17, sb.bytes_count()); // 10 + 1 (prefix) + 2 (key) + 4 (value) = 17 bytes
}

TEST(StringBucket, TraverseBucket)
{
    string_bucket<uint32_t> sb;
    construct_string_bucket(sb);

    auto it = sb.begin();
    check_key_and_value(it, "cat", 1);
    check_key_and_value(++it, "elephant", 3);
    check_key_and_value(++it, "rabbit", 5);
    check_key_and_value(++it, "bird", 7);
    check_key_and_value(++it, "horse", 9);
    EXPECT_EQ(sb.end(), ++it);
}

TEST(StringBucket, FindElementWithKey)
{
    string_bucket<uint32_t> sb;
    construct_string_bucket(sb);

    auto it1 = sb.find("rabbit");
    check_key_and_value(it1, "rabbit", 5);

    auto it2 = sb.find("horse");
    check_key_and_value(it2, "horse", 9);

    auto it3 = sb.find("tiger");
    EXPECT_EQ(sb.end(), it3);
}

TEST(StringBucket, AccessElement)
{
    string_bucket<uint32_t> sb;
    construct_string_bucket(sb);

    EXPECT_EQ(1, sb["cat"]);
    EXPECT_EQ(7, sb["bird"]);

    EXPECT_EQ(3, sb.at("elephant"));
    EXPECT_EQ(5, sb.at("rabbit"));

    EXPECT_THROW(sb.at("tiger"), out_of_range);
}

TEST(StringBucket, EraseElement)
{
    string_bucket<uint32_t> sb;
    construct_string_bucket(sb);

    auto it1 = sb.find("cat");
    auto next_it1 = sb.erase(it1);
    check_key_and_value(next_it1, "elephant", 3);

    auto it2 = sb.find("rabbit");
    auto next_it2 = sb.erase(it2);
    check_key_and_value(next_it2, "bird", 7);

    auto it3 = sb.find("horse");
    auto next_it = sb.erase(it3);
    EXPECT_EQ(sb.end(), next_it);

    auto it = sb.begin();
    check_key_and_value(it, "elephant", 3);
    check_key_and_value(++it, "bird", 7);
    EXPECT_EQ(sb.end(), ++it);
}

TEST(StringBucket, ClearBucket)
{
    string_bucket<uint32_t> sb;
    construct_string_bucket(sb);

    sb.clear();
    EXPECT_TRUE(sb.empty());
    EXPECT_EQ(0, sb.size());
    EXPECT_EQ(0, sb.bytes_count());
}
