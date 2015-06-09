/************************************************
 *  string_hashmap.hpp
 *  HAT-Trie
 *
 *  Copyright (c) 2014-2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef HAT_TRIE_STRING_HASHMAP_HPP_
#define HAT_TRIE_STRING_HASHMAP_HPP_

#include <array>
#include <utility>

#include "murmurhash.hpp"
#include "string_bucket.hpp"

namespace hat_trie
{

namespace impl
{

/************************************************
 * Declaration: class string_hashmap<T, N>
 ************************************************/

template <typename T, ::std::size_t N>
class string_hashmap
{
public: // Private Type(s)
    template <bool IsConst>
    class kv_iterator;
    typedef string_bucket<T> bucket_type;

public: // Public Type(s)
    typedef kv_iterator<false> iterator;
    typedef kv_iterator<true> const_iterator;
    typedef typename bucket_type::size_type size_type;
    typedef typename bucket_type::key_type key_type;
    typedef typename bucket_type::raw_key_type raw_key_type;
    typedef typename bucket_type::mapped_type mapped_type;
    typedef typename bucket_type::value_type value_type;

public: // Public Method(s)
    iterator begin(void);
    const_iterator begin(void) const;
    const_iterator cbegin(void) const;
    iterator end(void);
    const_iterator end(void) const;
    const_iterator cend(void) const;

    bool empty(void) const;
    size_type size(void) const;
    size_type bucket_count(void) const;
    size_type bucket_size(size_type i) const;
    size_type bucket(key_type const &key) const;

    ::std::pair<iterator, bool> insert(key_type const &key, mapped_type const &val);
    ::std::pair<iterator, bool> insert(key_type const &key, mapped_type &&val);
    template <typename ... Args>
    ::std::pair<iterator, bool> emplace(key_type const &key, Args&&... args);
    iterator erase(const_iterator const &it);
    void clear(void);

    iterator find(key_type const &key);
    const_iterator find(key_type const &key) const;
    mapped_type &at(key_type const &key);
    mapped_type const &at(key_type const &key) const;

    mapped_type &operator[](key_type const &key);
    mapped_type const &operator[](key_type const &key) const;

private: // Private Method(s)
    iterator make_iterator(size_type i, typename bucket_type::iterator it);
    const_iterator make_iterator(size_type i, typename bucket_type::const_iterator it) const;

private: // Private Property(ies)
    ::std::array<bucket_type, N> buckets_;
}; // class string_hashmap<T, N>

/************************************************
 * Declaration: class string_hashmap<T, N>::kv_iterator<B>
 ************************************************/

template <typename T, ::std::size_t N>
template <bool IsConst>
class string_hashmap<T, N>::kv_iterator
{
public: // Public Type(s)
    typedef ::std::forward_iterator_tag iterator_category;
    typedef value_type value_type;
    typedef typename ::std::conditional<IsConst, value_type const, value_type>::type *pointer;
    typedef typename ::std::conditional<IsConst, value_type const, value_type>::type &reference;
    typedef ::std::ptrdiff_t difference_type;

    typedef typename ::std::conditional
        <
            IsConst,
            typename ::std::array<bucket_type, N>::const_iterator,
            typename ::std::array<bucket_type, N>::iterator
        >::type bucket_array_iterator;
    typedef typename ::std::conditional
        <
            IsConst,
            typename bucket_type::const_iterator,
            typename bucket_type::iterator
        >::type bucket_iterator;

public: // Public Method(s)
    kv_iterator(void) = default;
    kv_iterator(bucket_array_iterator const &begin, bucket_array_iterator const &end);
    kv_iterator(bucket_array_iterator const &begin, bucket_array_iterator const &end,
                bucket_iterator const &sv_begin, bucket_iterator const & sv_end);

    kv_iterator &operator++(void);
    kv_iterator operator++(int);
    reference operator*(void) const;
    pointer operator->(void) const;
    bool operator==(kv_iterator const &it) const;
    bool operator!=(kv_iterator const &it) const;
    operator kv_iterator<true>(void) const;

private: // Private Method(s)
    void move_next(void);

    friend class string_hashmap;

private: // Private Property(ies)
    bucket_array_iterator bks_it_, bks_end_;
    bucket_iterator bk_it_, bk_end_;
}; // class string_hashmap<T, N>::kv_iterator<B>

/************************************************
 * Implementation: class string_hashmap<T, N>
 ************************************************/

template <typename T, ::std::size_t N>
inline typename string_hashmap<T, N>::iterator string_hashmap<T, N>::begin(void)
{
    return iterator(buckets_.begin(), buckets_.end());
}

template <typename T, ::std::size_t N>
inline typename string_hashmap<T, N>::const_iterator string_hashmap<T, N>::begin(void) const
{
    return const_iterator(buckets_.begin(), buckets_.end());
}

template <typename T, ::std::size_t N>
inline typename string_hashmap<T, N>::const_iterator string_hashmap<T, N>::cbegin(void) const
{
    return const_iterator(buckets_.begin(), buckets_.end());
}

template <typename T, ::std::size_t N>
inline typename string_hashmap<T, N>::iterator string_hashmap<T, N>::end(void)
{
    return iterator();
}

template <typename T, ::std::size_t N>
inline typename string_hashmap<T, N>::const_iterator string_hashmap<T, N>::end(void) const
{
    return const_iterator();
}

template <typename T, ::std::size_t N>
inline typename string_hashmap<T, N>::const_iterator string_hashmap<T, N>::cend(void) const
{
    return const_iterator();
}

template <typename T, ::std::size_t N>
inline bool string_hashmap<T, N>::empty(void) const
{
    return size() == 0;
}

template <typename T, ::std::size_t N>
inline typename string_hashmap<T, N>::size_type string_hashmap<T, N>::size(void) const
{
    size_type size = 0;
    for (auto const &bucket : buckets_)
    {
        size += bucket.size();
    }

    return size;
}

template <typename T, ::std::size_t N>
inline typename string_hashmap<T, N>::size_type string_hashmap<T, N>::bucket_count(void) const
{
    return N;
}

template <typename T, ::std::size_t N>
inline typename string_hashmap<T, N>::size_type string_hashmap<T, N>::bucket_size(size_type i) const
{
    return buckets_[i].size();
}

template <typename T, ::std::size_t N>
inline typename string_hashmap<T, N>::size_type string_hashmap<T, N>::bucket(key_type const &key) const
{
    return murmurhash3(key) % bucket_count();
}

template <typename T, ::std::size_t N>
::std::pair<typename string_hashmap<T, N>::iterator, bool> string_hashmap<T, N>::insert(key_type const &key, mapped_type const &val)
{
    return emplace(key, val);
}

template <typename T, ::std::size_t N>
::std::pair<typename string_hashmap<T, N>::iterator, bool> string_hashmap<T, N>::insert(key_type const &key, mapped_type &&val)
{
    return emplace(key, ::std::move(val));
}

template <typename T, ::std::size_t N>
template <typename... Args>
::std::pair<typename string_hashmap<T, N>::iterator, bool> string_hashmap<T, N>::emplace(key_type const &key, Args&&... args)
{
    auto bk_id = bucket(key);
    auto &bk = buckets_[bk_id];
    auto it = bk.find(key);
    auto is_exist = (it != bk.end());
    if (!is_exist)
    {
        it = bk.emplace_back(key, ::std::forward<Args>(args)...);
    }

    return ::std::make_pair(make_iterator(bk_id, it), !is_exist);
}

template <typename T, ::std::size_t N>
inline typename string_hashmap<T, N>::iterator string_hashmap<T, N>::erase(const_iterator const &it)
{
    auto bk_id = it.bks_it_ - buckets_.begin();
    auto &bk = buckets_[bk_id];
    auto result_it = bk.erase(it.bk_it_);
    return (result_it == bk.end())
        ? iterator(buckets_.begin() + bk_id + 1, buckets_.end())
        : make_iterator(bk_id, result_it);
}

template <typename T, ::std::size_t N>
void string_hashmap<T, N>::clear(void)
{
    for (auto &bucket : buckets_)
    {
        bucket.clear();
    }
}

template <typename T, ::std::size_t N>
typename string_hashmap<T, N>::iterator string_hashmap<T, N>::find(key_type const &key)
{
    auto bk_id = bucket(key);
    auto &bk = buckets_[bk_id];
    auto it = bk.find(key);
    return it != bk.end() ? make_iterator(bk_id, it) : end();
}

template <typename T, ::std::size_t N>
typename string_hashmap<T, N>::const_iterator string_hashmap<T, N>::find(key_type const &key) const
{
    auto bk_id = bucket(key);
    auto &bk = buckets_[bk_id];
    auto it = bk.find(key);
    return it != bk.end() ? make_iterator(bk_id, it) : end();
}

template <typename T, ::std::size_t N>
inline typename string_hashmap<T, N>::mapped_type &string_hashmap<T, N>::at(key_type const &key)
{
    auto it = find(key);
    if (it == end()) { throw ::std::out_of_range("string_hashmap::at: key not found"); }

    return it->get_value();
}

template <typename T, ::std::size_t N>
inline typename string_hashmap<T, N>::mapped_type const &string_hashmap<T, N>::at(key_type const &key) const
{
    auto it = find(key);
    if (it == end()) { throw ::std::out_of_range("string_hashmap::at: key not found"); }

    return it->get_value();
}

template <typename T, ::std::size_t N>
inline typename string_hashmap<T, N>::mapped_type &string_hashmap<T, N>::operator[](key_type const &key)
{
    auto it = find(key);
    return it->get_value();
}

template <typename T, ::std::size_t N>
inline typename string_hashmap<T, N>::mapped_type const &string_hashmap<T, N>::operator[](key_type const &key) const
{
    auto it = find(key);
    return it->get_value();
}

template <typename T, ::std::size_t N>
inline typename string_hashmap<T, N>::iterator string_hashmap<T, N>::make_iterator(size_type i, typename bucket_type::iterator it)
{
    auto it_end = buckets_[i].end();
    return it != it_end
        ? iterator(buckets_.begin() + i, buckets_.end(), it, it_end)
        : iterator(buckets_.begin() + i + 1, buckets_.end());
}

template <typename T, ::std::size_t N>
inline typename string_hashmap<T, N>::const_iterator string_hashmap<T, N>::make_iterator(size_type i, typename bucket_type::const_iterator it) const
{
    auto it_end = buckets_[i].end();
    return it != it_end
        ? const_iterator(buckets_.begin() + i, buckets_.end(), it, it_end)
        : iterator(buckets_.begin() + i + 1, buckets_.end());
}

/************************************************
 * Implementation: class string_hashmap<T, N>::kv_iterator<B>
 ************************************************/

template <typename T, ::std::size_t N>
template <bool B>
inline string_hashmap<T, N>::kv_iterator<B>::kv_iterator(
        bucket_array_iterator const &bks_begin,
        bucket_array_iterator const &bks_end)
    : bks_it_(bks_begin), bks_end_(bks_end)
{
    move_next();
}

template <typename T, ::std::size_t N>
template <bool B>
inline string_hashmap<T, N>::kv_iterator<B>::kv_iterator(
        bucket_array_iterator const &bks_begin,
        bucket_array_iterator const &bks_end,
        bucket_iterator const &bk_begin,
        bucket_iterator const &bk_end)
    : bks_it_(bks_begin), bks_end_(bks_end), bk_it_(bk_begin), bk_end_(bk_end)
{
    // do nothing
}

template <typename T, ::std::size_t N>
template <bool B>
inline string_hashmap<T, N>::kv_iterator<B> &string_hashmap<T, N>::kv_iterator<B>::operator++(void)
{
    if (++bk_it_ == bk_end_)
    {
        ++bks_it_;
        move_next();
    }

    return *this;
}

template <typename T, ::std::size_t N>
template <bool B>
inline typename string_hashmap<T, N>::template kv_iterator<B>::kv_iterator string_hashmap<T, N>::kv_iterator<B>::operator++(int)
{
    auto it = *this;
    this->operator++();
    return it;
}

template <typename T, ::std::size_t N>
template <bool B>
inline typename string_hashmap<T, N>::template kv_iterator<B>::reference string_hashmap<T, N>::kv_iterator<B>::operator*(void) const
{
    return *bk_it_;
}

template <typename T, ::std::size_t N>
template <bool B>
inline typename string_hashmap<T, N>::template kv_iterator<B>::pointer string_hashmap<T, N>::kv_iterator<B>::operator->(void) const
{
    return &(this->operator*());
}

template <typename T, ::std::size_t N>
template <bool B>
inline bool string_hashmap<T, N>::kv_iterator<B>::operator==(kv_iterator const &it) const
{
    return bk_it_ == it.bk_it_;
}

template <typename T, ::std::size_t N>
template <bool B>
inline bool string_hashmap<T, N>::kv_iterator<B>::operator!=(kv_iterator const &it) const
{
    return !(this->operator==(it));
}

template <typename T, ::std::size_t N>
template <bool B>
string_hashmap<T, N>::kv_iterator<B>::operator kv_iterator<true>(void) const
{
    return kv_iterator<true>(bks_it_, bks_end_, bk_it_, bk_end_);
}

template <typename T, ::std::size_t N>
template <bool B>
inline void string_hashmap<T, N>::kv_iterator<B>::move_next(void)
{
    while (bks_it_ != bks_end_)
    {
        bk_it_ = bks_it_->begin();
        bk_end_ = bks_it_->end();
        if (bk_it_ != bk_end_)
        {
            return;
        }

        ++bks_it_;
    }

    bk_it_ = bk_end_ = bucket_iterator();
}

} // namespace impl

} // namespace hat_trie

#endif // HAT_TRIE_STRING_HASHMAP_HPP_
