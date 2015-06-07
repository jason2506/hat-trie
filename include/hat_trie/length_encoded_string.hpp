/************************************************
 *  length_encoded_string.hpp
 *  HAT-Trie
 *
 *  Copyright (c) 2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef HAT_TRIE_LENGTH_ENCODED_STRING_HPP_
#define HAT_TRIE_LENGTH_ENCODED_STRING_HPP_

#include <cassert>
#include <climits>
#include <cstddef>
#include <cstring>
#include <string>

namespace hat_trie
{

namespace impl
{

/************************************************
 * Declaration: Helper Function(s)
 ************************************************/

::std::size_t prefix_size(::std::size_t n);
void encode_string(char *dest, ::std::string src);
void encode_string(char *dest, char const *src, ::std::size_t n);

static void check_length(::std::size_t n);

/************************************************
 * Declaration: class length_encoded_string
 ************************************************/

class length_encoded_string
{
public: // Public Type(s)
    typedef ::std::size_t size_type;
    typedef char value_type;
    typedef char const *pointer;

public: // Public Method(s)
    explicit length_encoded_string(char const *s);

    size_type size(void) const;
    size_type prefix_size(void) const;
    size_type bytes_count(void) const;

    pointer begin(void) const;
    pointer end(void) const;

    pointer data(void) const;

private: // Private Property(ies)
    char const *s_;
}; // class length_encoded_string

/************************************************
 * Implementation: class length_encoded_string
 ************************************************/

inline length_encoded_string::length_encoded_string(char const *s)
    : s_(s)
{
    // do nothing
}

inline length_encoded_string::size_type length_encoded_string::size(void) const
{
    return *s_;
}

inline length_encoded_string::size_type length_encoded_string::prefix_size(void) const
{
    return 1;
}

inline length_encoded_string::size_type length_encoded_string::bytes_count(void) const
{
    return prefix_size() + size();
}

inline length_encoded_string::pointer length_encoded_string::begin(void) const
{
    return s_ + prefix_size();
}

inline length_encoded_string::pointer length_encoded_string::end(void) const
{
    return s_ + prefix_size() + size();
}

inline length_encoded_string::pointer length_encoded_string::data(void) const
{
    return s_;
}

/************************************************
 * Implementation: Helper Function(s)
 ************************************************/

inline ::std::size_t prefix_size(::std::size_t n)
{
    check_length(n);
    return 1;
}

inline void encode_string(char *dest, ::std::string src)
{
    encode_string(dest, src.data(), src.size());
}

inline void encode_string(char *dest, char const *src, ::std::size_t n)
{
    check_length(n);

    *dest = n;
    ::std::memcpy(++dest, src, n);
}

inline void check_length(::std::size_t n)
{
    // currently, we use only one byte to store the length of string
    assert(n < (1 << CHAR_BIT));
}

} // namespace impl

} // namespace hat_trie

#endif // HAT_TRIE_LENGTH_ENCODED_STRING_HPP_
