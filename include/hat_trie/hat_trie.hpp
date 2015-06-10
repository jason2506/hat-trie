/************************************************
 *  hat_trie.hpp
 *  HAT-Trie
 *
 *  Copyright (c) 2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef HAT_TRIE_HAT_TRIE_HPP_
#define HAT_TRIE_HAT_TRIE_HPP_

#include <cassert>
#include <array>
#include <memory>

#include "string_hashmap.hpp"

namespace hat_trie
{

/************************************************
 * Declaration: class hat_trie<T, N, M>
 ************************************************/

template <typename T, ::std::size_t N, ::std::size_t M>
class hat_trie
{
public: // Private Type(s)
    typedef impl::string_hashmap<T, N> hashmap_type;

public: // Public Type(s)
    typedef typename hashmap_type::size_type size_type;
    typedef typename hashmap_type::key_type key_type;
    typedef typename hashmap_type::raw_key_type raw_key_type;
    typedef typename hashmap_type::mapped_type mapped_type;
    typedef typename hashmap_type::value_type value_type;

public: // Public Method(s)
    hat_trie(void);
    ~hat_trie(void);

private: // Private Type(s)
    class node;
    class internal_node;
    class leaf_node;
    struct node_deleter;
    typedef ::std::unique_ptr<node, node_deleter> node_ptr;

private: // Private Property(ies)
    static constexpr ::std::size_t NUM_CHARS = 2 ^ CHAR_BIT;
    static constexpr ::std::size_t NUM_CHILDREN = NUM_CHARS + 1;

    node_ptr root_;
}; // class hat_trie<T, N, M>

/************************************************
 * Declaration: class hat_trie<T, N, M>::node
 ************************************************/

template <typename T, ::std::size_t N, ::std::size_t M>
class hat_trie<T, N, M>::node
{
public: // Public Type(s)
    enum type_t { INTERNAL, PURE_BUCKET, HYBRID_BUCKET };

public: // Public Method(s)
    type_t get_type(void) const;
    internal_node &as_internal_node(void);
    internal_node const &as_internal_node(void) const;
    leaf_node &as_leaf_node(void);
    leaf_node const &as_leaf_node(void) const;

protected: // Protected Method(s)
    explicit node(type_t type);
    ~node(void);

    void to_hybrid(void);

private: // Private Property(ies)
    type_t type_;
}; // class hat_trie<T, N, M>::node

/************************************************
 * Declaration: class hat_trie<T, N, M>::internal_node
 ************************************************/

template <typename T, ::std::size_t N, ::std::size_t M>
class hat_trie<T, N, M>::internal_node : public node
{
public: // Public Method(s)
    internal_node(void);

private: // Private Property(ies)
    ::std::array<node_ptr, NUM_CHILDREN> children_;
}; // class hat_trie<T, N, M>::internal_node

/************************************************
 * Declaration: class hat_trie<T, N, M>::leaf_node
 ************************************************/

template <typename T, ::std::size_t N, ::std::size_t M>
class hat_trie<T, N, M>::leaf_node : public node
{
public: // Public Method(s)
    explicit leaf_node(bool is_pure);
    leaf_node(bool is_pure, char l, char u);

private: // Private Property(ies)
    hashmap_type hashmap_;
    char l_, u_;
}; // class hat_trie<T, N, M>::leaf_node

/************************************************
 * Declaration: struct hat_trie<T, N, M>::node_deleter
 ************************************************/

template <typename T, ::std::size_t N, ::std::size_t M>
struct hat_trie<T, N, M>::node_deleter
{
    void operator()(node *ptr);
}; // struct hat_trie<T, N, M>::node_deleter

/************************************************
 * Implementation: class hat_trie<T, N, M>
 ************************************************/

template <typename T, ::std::size_t N, ::std::size_t M>
inline hat_trie<T, N, M>::hat_trie(void)
{
    // do nothing
}

template <typename T, ::std::size_t N, ::std::size_t M>
inline hat_trie<T, N, M>::~hat_trie(void)
{
    // do nothing
}

/************************************************
 * Implementation: class hat_trie<T, N, M>::node
 ************************************************/

template <typename T, ::std::size_t N, ::std::size_t M>
inline hat_trie<T, N, M>::node::node(type_t type)
    : type_(type)
{
    // do nothing
}

template <typename T, ::std::size_t N, ::std::size_t M>
inline hat_trie<T, N, M>::node::~node(void)
{
    // do nothing
}

template <typename T, ::std::size_t N, ::std::size_t M>
inline typename hat_trie<T, N, M>::node::type_t hat_trie<T, N, M>::node::get_type(void) const
{
    return type_;
}

template <typename T, ::std::size_t N, ::std::size_t M>
inline typename hat_trie<T, N, M>::internal_node &hat_trie<T, N, M>::node::as_internal_node(void)
{
    assert(type_ == INTERNAL);
    return *reinterpret_cast<internal_node *>(this);
}

template <typename T, ::std::size_t N, ::std::size_t M>
inline typename hat_trie<T, N, M>::internal_node const &hat_trie<T, N, M>::node::as_internal_node(void) const
{
    assert(type_ == INTERNAL);
    return *reinterpret_cast<internal_node const *>(this);
}

template <typename T, ::std::size_t N, ::std::size_t M>
inline typename hat_trie<T, N, M>::leaf_node &hat_trie<T, N, M>::node::as_leaf_node(void)
{
    assert(type_ == PURE_BUCKET || type_ == HYBRID_BUCKET);
    return *reinterpret_cast<leaf_node *>(this);
}

template <typename T, ::std::size_t N, ::std::size_t M>
inline typename hat_trie<T, N, M>::leaf_node const &hat_trie<T, N, M>::node::as_leaf_node(void) const
{
    assert(type_ == PURE_BUCKET || type_ == HYBRID_BUCKET);
    return *reinterpret_cast<leaf_node const *>(this);
}

template <typename T, ::std::size_t N, ::std::size_t M>
inline void hat_trie<T, N, M>::node::to_hybrid(void)
{
    assert(type_ == PURE_BUCKET);
    type_ = HYBRID_BUCKET;
}

/************************************************
 * Implementation: class hat_trie<T, N, M>::internal_node
 ************************************************/

template <typename T, ::std::size_t N, ::std::size_t M>
inline hat_trie<T, N, M>::internal_node::internal_node(void)
    : node(node::INTERNAL)
{
    // do nothing
}

/************************************************
 * Implementation: class hat_trie<T, N, M>::leaf_node
 ************************************************/

template <typename T, ::std::size_t N, ::std::size_t M>
inline hat_trie<T, N, M>::leaf_node::leaf_node(bool is_pure)
    : leaf_node(is_pure, 0, NUM_CHARS - 1)
{
    // do nothing
}

template <typename T, ::std::size_t N, ::std::size_t M>
inline hat_trie<T, N, M>::leaf_node::leaf_node(bool is_pure, char l, char u)
    : node(is_pure ? node::PURE_BUCKET : node::HYBRID_BUCKET), l_(l), u_(u)
{
    // do nothing
}

/************************************************
 * Implementation: struct hat_trie<T, N, M>::node_deleter
 ************************************************/

template <typename T, ::std::size_t N, ::std::size_t M>
void hat_trie<T, N, M>::node_deleter::operator()(node *ptr)
{
    if (ptr->get_type() == node::INTERNAL)
    {
        delete &(ptr->as_internal_node());
    }
    else
    {
        delete &(ptr->as_leaf_node());
    }
}

} // namespace hat_trie

#endif // HAT_TRIE_HAT_TRIE_HPP_
