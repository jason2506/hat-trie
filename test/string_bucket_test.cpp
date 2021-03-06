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

struct some_type
{
    explicit some_type(bool &alive) : alive(alive)
                        { alive = true; }
    ~some_type(void)    { alive = false; }
    bool &alive;
};

struct spy
{
    size_t move_count, copy_count;
};

struct some_type_with_non_trival_copy_constructor
{
    explicit some_type_with_non_trival_copy_constructor(spy &c) : c(c)
        { c.move_count = c.copy_count = 0; }
    some_type_with_non_trival_copy_constructor(
            some_type_with_non_trival_copy_constructor const &obj) : c(obj.c)
        { c.copy_count++; }
    some_type_with_non_trival_copy_constructor(
            some_type_with_non_trival_copy_constructor &&obj) : c(obj.c)
        { c.move_count++; }

    spy &c;
};

struct some_type_with_noexcept_move_constructor
{
    explicit some_type_with_noexcept_move_constructor(spy &c) : c(c)
        { c.move_count = c.copy_count = 0; }
    some_type_with_noexcept_move_constructor(
            some_type_with_noexcept_move_constructor const &obj) : c(obj.c)
        { c.copy_count++; }
    some_type_with_noexcept_move_constructor(
            some_type_with_noexcept_move_constructor &&obj) noexcept : c(obj.c)
        { c.move_count++; }

    spy &c;
};

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
    EXPECT_EQ(sb.end(), sb.begin());
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

    EXPECT_EQ(2, sb.size());

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
    EXPECT_EQ(sb.end(), sb.begin());
}

TEST(StringBucket, ConstructAndDestructObject)
{
    bool alive = false;
    {
        string_bucket<some_type> sb;
        sb.emplace_back("key", alive);
        EXPECT_TRUE(alive);
    }

    EXPECT_FALSE(alive);
}

TEST(StringBucket, DestructObjectWhenErasingElement)
{
    string_bucket<some_type> sb;
    bool alive = false;
    auto it = sb.emplace_back("key", alive);
    sb.erase(it);
    EXPECT_FALSE(alive);
}

TEST(StringBucket, InsertRValueReference)
{
    string_bucket<some_type_with_non_trival_copy_constructor> sb;
    spy c;
    sb.push_back("key", some_type_with_non_trival_copy_constructor(c));
    EXPECT_EQ(0, c.copy_count);
    EXPECT_EQ(1, c.move_count);
}

TEST(StringBucket, InsertLValueReference)
{
    string_bucket<some_type_with_non_trival_copy_constructor> sb;
    spy c;
    some_type_with_non_trival_copy_constructor obj(c);
    sb.push_back("key", obj);
    EXPECT_EQ(1, c.copy_count);
    EXPECT_EQ(0, c.move_count);
}

TEST(StringBucket, CopyObjectWhenReallocatingMemory)
{
    string_bucket<some_type_with_non_trival_copy_constructor> sb;
    spy c1, c2;
    sb.emplace_back("key1", c1);
    sb.emplace_back("key2", c2);
    EXPECT_EQ(1, c1.copy_count);
    EXPECT_EQ(0, c1.move_count);
}

TEST(StringBucket, MoveObjectWhenReallocatingMemory)
{
    string_bucket<some_type_with_noexcept_move_constructor> sb;
    spy c1, c2;
    sb.emplace_back("key1", c1);
    sb.emplace_back("key2", c2);
    EXPECT_EQ(0, c1.copy_count);
    EXPECT_EQ(1, c1.move_count);
}
