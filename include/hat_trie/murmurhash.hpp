// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#ifndef _MURMURHASH3_HPP_
#define _MURMURHASH3_HPP_

#include <cassert>
#include <cstdint>
#include <limits>
#include <string>

static uint32_t const MURMURHASH_SEED = 0x13579bdf;

static inline ::std::uint32_t rotl32(::std::uint32_t x, ::std::int8_t r)
{
    return (x << r) | (x >> (32 - r));
}

static inline ::std::uint32_t fmix32(::std::uint32_t h)
{
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

::std::uint8_t murmurhash3(::std::string const &key)
{
    assert(key.size() <= ::std::numeric_limits<int>::max());

    auto data = reinterpret_cast<::std::uint8_t const *>(key.data());
    auto size = static_cast<int>(key.size());
    auto nblocks = size / 4;

    auto h1 = MURMURHASH_SEED;
    ::std::uint32_t const c1 = 0xcc9e2d51;
    ::std::uint32_t const c2 = 0x1b873593;

    // body
    auto blocks = reinterpret_cast<uint32_t const *>(data + nblocks * 4);
    for(int i = -nblocks; i; ++i)
    {
        auto k1 = blocks[i];

        k1 *= c1;
        k1 = rotl32(k1, 15);
        k1 *= c2;

        h1 ^= k1;
        h1 = rotl32(h1, 13); 
        h1 = h1 * 5 + 0xe6546b64;
    }

    // tail
    auto tail = reinterpret_cast<uint8_t const *>(data + nblocks * 4);
    ::std::uint32_t k1 = 0;
    switch (size & 3)
    {
        case 3: k1 ^= tail[2] << 16;
        case 2: k1 ^= tail[1] << 8;
        case 1: k1 ^= tail[0];
                k1 *= c1;
                k1 = rotl32(k1, 15);
                k1 *= c2;
                h1 ^= k1;
    }

    // finalization
    h1 ^= size;
    h1 = fmix32(h1);
    return h1;
}

#endif // _MURMURHASH3_HPP_
