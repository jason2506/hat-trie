/************************************************
 *  string_bucket.hpp
 *  HAT-Trie
 *
 *  Copyright (c) 2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef HAT_TRIE_STRING_BUCKET_HPP_
#define HAT_TRIE_STRING_BUCKET_HPP_

#include <new>
#include <stdexcept>
#include <type_traits>

#include "length_encoded_string.hpp"

namespace hat_trie
{

namespace impl
{

/************************************************
 * Declaration: class string_bucket<T>
 ************************************************/

template <typename T>
class string_bucket
{
private: // Private Type(s)
    template <bool IsConst>
    class kv_iterator;

public: // Public Type(s)
    class key_and_value;
    typedef kv_iterator<false> iterator;
    typedef kv_iterator<true> const_iterator;
    typedef ::std::size_t size_type;
    typedef ::std::string key_type;
    typedef length_encoded_string raw_key_type;
    typedef T mapped_type;
    typedef key_and_value value_type;

public: // Public Method(s)
    string_bucket(void);
    string_bucket(string_bucket const &sb) = delete;
    string_bucket(string_bucket &&sb);
    ~string_bucket(void);

    iterator begin(void);
    const_iterator begin(void) const;
    const_iterator cbegin(void) const;
    iterator end(void);
    const_iterator end(void) const;
    const_iterator cend(void) const;

    bool empty(void) const;
    size_type size(void) const;
    size_type bytes_count(void) const;

    iterator push_back(key_type const &key, mapped_type const &value);
    iterator push_back(key_type const &key, mapped_type &&value);
    template <typename ... Args>
    iterator emplace_back(key_type const &key, Args&&... args);
    iterator erase(const_iterator const &it);
    void clear(void);

    iterator find(key_type const &key);
    const_iterator find(key_type const &key) const;
    mapped_type &at(key_type const &key);
    mapped_type const &at(key_type const &key) const;

    mapped_type &operator[](key_type const &key);
    mapped_type const &operator[](key_type const &key) const;
    string_bucket &operator=(string_bucket const &sb) = delete;
    string_bucket &operator=(string_bucket &&sb);

private: // Private Method(s)
    void move_elements(char *dist, char *src, size_type n);

private: // Private Property(ies)
    size_type num_strings_;
    size_type num_bytes_;
    char *s_;
}; // class string_bucket<T>

/************************************************
 * Declaration: class string_bucket<T>::kv_iterator<B>
 ************************************************/

template <typename T>
template <bool IsConst>
class string_bucket<T>::kv_iterator
{
public: // Public Type(s)
    typedef ::std::forward_iterator_tag iterator_category;
    typedef value_type value_type;
    typedef typename ::std::conditional<IsConst, value_type const, value_type>::type *pointer;
    typedef typename ::std::conditional<IsConst, value_type const, value_type>::type &reference;
    typedef ::std::ptrdiff_t difference_type;

public: // Public Method(s)
    kv_iterator(void);
    explicit kv_iterator(char *ptr);

    difference_type raw_distance(kv_iterator const &it) const;

    kv_iterator &operator++(void);
    kv_iterator operator++(int);
    reference operator*(void) const;
    pointer operator->(void) const;
    bool operator==(kv_iterator const &it) const;
    bool operator!=(kv_iterator const &it) const;
    operator kv_iterator<true>(void) const;

private: // Private Property(ies)
    value_type kv_;
}; // class string_bucket<T>::kv_iterator<B>

/************************************************
 * Declaration: class string_bucket<T>::key_and_value
 ************************************************/

template <typename T>
class string_bucket<T>::key_and_value
{
public: // Public Method(s)
    explicit key_and_value(char *ptr);

    raw_key_type get_key(void) const;
    mapped_type const &get_value(void) const;
    mapped_type &get_value(void);

    template <bool B>
    friend class kv_iterator;

private: // Private Property(ies)
    char *ptr_;
}; // class string_bucket<T>::key_and_value

/************************************************
 * Implementation: class string_bucket<T>
 ************************************************/

template <typename T>
inline string_bucket<T>::string_bucket(void)
    : num_strings_(0), num_bytes_(0), s_(nullptr)
{
    // do nothing
}

template <typename T>
inline string_bucket<T>::string_bucket(string_bucket &&sb)
    : num_strings_(sb.num_strings_), num_bytes_(sb.num_bytes_), s_(sb.s_)
{
    sb.num_strings_ = sb.num_bytes_ = 0;
    sb.s_ = nullptr;
}

template <typename T>
inline string_bucket<T>::~string_bucket(void)
{
    clear();
}

template <typename T>
inline typename string_bucket<T>::iterator string_bucket<T>::begin(void)
{
    return iterator(s_);
}

template <typename T>
inline typename string_bucket<T>::const_iterator string_bucket<T>::begin(void) const
{
    return const_iterator(s_);
}

template <typename T>
inline typename string_bucket<T>::const_iterator string_bucket<T>::cbegin(void) const
{
    return const_iterator(s_);
}

template <typename T>
inline typename string_bucket<T>::iterator string_bucket<T>::end(void)
{
    return iterator(s_ + num_bytes_);
}

template <typename T>
inline typename string_bucket<T>::const_iterator string_bucket<T>::end(void) const
{
    return const_iterator(s_ + num_bytes_);
}

template <typename T>
inline typename string_bucket<T>::const_iterator string_bucket<T>::cend(void) const
{
    return const_iterator(s_ + num_bytes_);
}

template <typename T>
inline bool string_bucket<T>::empty(void) const
{
    return size() == 0;
}

template <typename T>
inline typename string_bucket<T>::size_type string_bucket<T>::size(void) const
{
    return num_strings_;
}

template <typename T>
inline typename string_bucket<T>::size_type string_bucket<T>::bytes_count(void) const
{
    return num_bytes_;
}

template <typename T>
inline typename string_bucket<T>::iterator string_bucket<T>::push_back(key_type const &key, mapped_type const &value)
{
    return emplace_back(key, value);
}

template <typename T>
inline typename string_bucket<T>::iterator string_bucket<T>::push_back(key_type const &key, mapped_type &&value)
{
    return emplace_back(key, ::std::move(value));
}

template <typename T>
template <typename... Args>
typename string_bucket<T>::iterator string_bucket<T>::emplace_back(key_type const &key, Args&&... args)
{
    auto n = key.size();
    auto new_size = num_bytes_ + prefix_size(n) + n + sizeof(mapped_type);
    auto new_s = static_cast<char *>(::operator new(new_size));
    move_elements(new_s, s_, num_bytes_);
    ::operator delete(s_);
    s_ = new_s;

    encode_string(s_ + num_bytes_, key);
    ++num_strings_;

    iterator it(s_ + num_bytes_);
    auto ptr = &(it->get_value());
    new(ptr) mapped_type(::std::forward<Args>(args)...);

    num_bytes_ = new_size;
    return it;
}

template <typename T>
inline typename string_bucket<T>::iterator string_bucket<T>::erase(const_iterator const &it)
{
    auto key = it->get_key();
    auto &val = it->get_value();
    val.~mapped_type();

    auto offset = it.raw_distance(begin());
    if (num_strings_ > 1)
    {
        auto kv_size = key.prefix_size() + key.size() + sizeof(mapped_type);
        auto new_s = static_cast<char *>(::operator new(num_bytes_ - kv_size));
        move_elements(new_s, s_, offset);
        move_elements(new_s + offset, s_ + offset + kv_size,
                      num_bytes_ - offset - kv_size);

        ::operator delete(s_);
        s_ = new_s;

        num_bytes_ -= kv_size;
        --num_strings_;
    }
    else
    {
        assert(offset == 0);

        ::operator delete(s_);
        num_bytes_ = num_strings_ = 0;
        s_ = nullptr;
    }

    return iterator(s_ + offset);
}

template <typename T>
void string_bucket<T>::clear(void)
{
    if (s_)
    {
        auto it = begin();
        while (it != end())
        {
            it->get_value().~mapped_type();
            ++it;
        }

        ::operator delete(s_);
    }

    num_strings_ = num_bytes_ = 0;
    s_ = nullptr;
}

template <typename T>
typename string_bucket<T>::iterator string_bucket<T>::find(key_type const &key)
{
    auto it = begin();
    while (it != end())
    {
        auto it_key = it->get_key();
        auto length = it_key.size();
        if (length == key.size() && ::std::memcmp(key.data(), it_key.begin(), length) == 0)
        {
            break;
        }

        ++it;
    }

    return it;
}

template <typename T>
typename string_bucket<T>::const_iterator string_bucket<T>::find(key_type const &key) const
{
    auto that = const_cast<string_bucket *>(this);
    auto begin_it = that->begin(), it = that->find(key);
    return const_iterator(s_ + it.raw_distance(begin_it));
}

template <typename T>
inline typename string_bucket<T>::mapped_type &string_bucket<T>::at(key_type const &key)
{
    auto it = find(key);
    if (it == end()) { throw ::std::out_of_range("string_bucket::at: key not found"); }

    return it->get_value();
}

template <typename T>
inline typename string_bucket<T>::mapped_type const &string_bucket<T>::at(key_type const &key) const
{
    auto it = find(key);
    if (it == end()) { throw ::std::out_of_range("string_bucket::at: key not found"); }

    return it->get_value();
}

template <typename T>
inline typename string_bucket<T>::mapped_type &string_bucket<T>::operator[](key_type const &key)
{
    auto it = find(key);
    return it->get_value();
}

template <typename T>
inline typename string_bucket<T>::mapped_type const &string_bucket<T>::operator[](key_type const &key) const
{
    auto it = find(key);
    return it->get_value();
}

template <typename T>
inline string_bucket<T> &string_bucket<T>::operator=(string_bucket &&sb)
{
    if (this != &sb)
    {
        clear();

        num_strings_ = sb.num_strings_;
        num_bytes_ = sb.num_bytes_;
        s_ = sb.s_;

        sb.num_strings_ = sb.num_bytes_ = 0;
        sb.s_ = nullptr;
    }

    return *this;
}

template <typename T>
void string_bucket<T>::move_elements(char *dist, char *src, size_type n)
{
    ::std::memcpy(dist, src, n);
    if (!::std::is_trivially_copyable<T>::value)
    {
        auto ptr = dist;
        iterator it(src), end_it(src + n);
        while (it != end_it)
        {
            ptr += it->get_key().bytes_count();
            new(ptr) mapped_type(::std::move_if_noexcept(it->get_value()));
            it->get_value().~mapped_type();

            ptr += sizeof(mapped_type);
            ++it;
        }
    }
}

/************************************************
 * Implementation: class string_bucket<T>::kv_iterator<B>
 ************************************************/

template <typename T>
template <bool B>
inline string_bucket<T>::kv_iterator<B>::kv_iterator(void)
    : kv_(nullptr)
{
    // do nothing
}

template <typename T>
template <bool B>
inline string_bucket<T>::kv_iterator<B>::kv_iterator(char *ptr)
    : kv_(ptr)
{
    // do nothing
}

template <typename T>
template <bool B>
inline typename string_bucket<T>::template kv_iterator<B>::difference_type string_bucket<T>::kv_iterator<B>::raw_distance(kv_iterator const &it) const
{
    return kv_.ptr_ - it.kv_.ptr_;
}

template <typename T>
template <bool B>
inline string_bucket<T>::kv_iterator<B> &string_bucket<T>::kv_iterator<B>::operator++(void)
{
    auto key = kv_.get_key();
    kv_.ptr_ += key.prefix_size() + key.size() + sizeof(mapped_type);
    return *this;
}

template <typename T>
template <bool B>
inline typename string_bucket<T>::template kv_iterator<B>::kv_iterator string_bucket<T>::kv_iterator<B>::operator++(int)
{
    auto it = *this;
    this->operator++();
    return it;
}

template <typename T>
template <bool B>
inline typename string_bucket<T>::template kv_iterator<B>::reference string_bucket<T>::kv_iterator<B>::operator*(void) const
{
    return const_cast<reference>(kv_);
}

template <typename T>
template <bool B>
inline typename string_bucket<T>::template kv_iterator<B>::pointer string_bucket<T>::kv_iterator<B>::operator->(void) const
{
    return &(this->operator*());
}

template <typename T>
template <bool B>
inline bool string_bucket<T>::kv_iterator<B>::operator==(kv_iterator const &it) const
{
    return kv_.ptr_ == it.kv_.ptr_;
}

template <typename T>
template <bool B>
inline bool string_bucket<T>::kv_iterator<B>::operator!=(kv_iterator const &it) const
{
    return !(this->operator==(it));
}

template <typename T>
template <bool B>
string_bucket<T>::kv_iterator<B>::operator kv_iterator<true>(void) const
{
    return kv_iterator<true>(kv_.ptr_);
}

/************************************************
 * Implementation: class string_bucket<T>::key_and_value
 ************************************************/

template <typename T>
inline string_bucket<T>::key_and_value::key_and_value(char *ptr)
    : ptr_(ptr)
{
    // do nothing
}

template <typename T>
inline typename string_bucket<T>::raw_key_type string_bucket<T>::key_and_value::get_key(void) const
{
    return raw_key_type(ptr_);
}

template <typename T>
inline typename string_bucket<T>::mapped_type const &string_bucket<T>::key_and_value::get_value(void) const
{
    raw_key_type key(ptr_);
    auto ptr = ptr_ + key.size() + key.prefix_size();
    return *reinterpret_cast<mapped_type *>(ptr);
}

template <typename T>
inline typename string_bucket<T>::mapped_type &string_bucket<T>::key_and_value::get_value(void)
{
    raw_key_type key(ptr_);
    auto ptr = ptr_ + key.size() + key.prefix_size();
    return *reinterpret_cast<mapped_type *>(ptr);
}

} // namespace impl

} // namespace hat_trie

#endif // HAT_TRIE_STRING_BUCKET_HPP_
