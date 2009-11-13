//  tagged pointer, for aba prevention
//
//  Copyright (C) 2008, 2009 Tim Blechmann, based on code by Cory Nelson
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  Disclaimer: Not a Boost library.

#ifndef BOOST_LOCKFREE_TAGGED_PTR_PTRCOMPRESSION_HPP_INCLUDED
#define BOOST_LOCKFREE_TAGGED_PTR_PTRCOMPRESSION_HPP_INCLUDED

#include <boost/lockfree/detail/cas.hpp>
#include <boost/lockfree/detail/branch_hints.hpp>

#include <cstddef>              /* for std::size_t */

#include <boost/cstdint.hpp>

namespace boost
{
namespace lockfree
{

#if defined (__x86_64__) || defined (_M_X64)

template <class T>
class BOOST_LOCKFREE_DCAS_ALIGNMENT tagged_ptr
{
    typedef boost::uint64_t compressed_ptr_t;
    typedef boost::uint16_t tag_t;

private:
    union cast_unit
    {
        compressed_ptr_t value;
        tag_t tag[4];
    };

    static const int tag_index = 3;
    static const compressed_ptr_t ptr_mask = 0xffffffffffff; //(1L<<48L)-1;

    static T* extract_ptr(compressed_ptr_t const & i)
    {
        return (T*)(i & ptr_mask);
    }

    static tag_t extract_tag(compressed_ptr_t const & i)
    {
        cast_unit cu;
        cu.value = i;
        return cu.tag[tag_index];
    }

    static compressed_ptr_t pack_ptr(T * ptr, int tag)
    {
        cast_unit ret;
        ret.value = compressed_ptr_t(ptr);
        ret.tag[tag_index] = tag;
        return ret.value;
    }

public:
    static const bool is_lockfree = boost::lockfree::atomic_cas<compressed_ptr_t>::is_lockfree;

    /** uninitialized constructor */
    tagged_ptr(void)//: ptr(0), tag(0)
    {}

    /** copy constructor */
    tagged_ptr(tagged_ptr const & p)//: ptr(0), tag(0)
    {
        set(p);
    }

    explicit tagged_ptr(T * p, tag_t t = 0):
        ptr(pack_ptr(p, t))
    {}

    /** atomic set operations */
    /* @{ */
    void operator= (tagged_ptr const & p)
    {
        atomic_set(p);
    }

    void atomic_set(tagged_ptr const & p)
    {
        set(p);
    }

    void atomic_set(T * p, tag_t t)
    {
        ptr = pack_ptr(p, t);
    }
    /* @} */

    /** unsafe set operation */
    /* @{ */
    void set(tagged_ptr const & p)
    {
        ptr = p.ptr;
    }

    void set(T * p, tag_t t)
    {
        ptr = pack_ptr(p, t);
    }
    /* @} */

    /** comparing semantics */
    /* @{ */
    bool operator== (tagged_ptr const & p) const
    {
        return (ptr == p.ptr);
    }

    bool operator!= (tagged_ptr const & p) const
    {
        return !operator==(p);
    }
    /* @} */

    /** pointer access */
    /* @{ */
    T * get_ptr() const
    {
        return extract_ptr(ptr);
    }

    void set_ptr(T * p)
    {
        tag_t tag = get_tag();
        ptr = pack_ptr(p, tag);
    }
    /* @} */

    /** tag access */
    /* @{ */
    tag_t get_tag() const
    {
        return extract_tag(ptr);
    }

    void set_tag(tag_t t)
    {
        T * p = get_ptr();
        ptr = pack_ptr(p, t);
    }
    /* @} */

    /** compare and swap  */
    /* @{ */
private:
    bool cas(compressed_ptr_t const & oldval, compressed_ptr_t const & newval)
    {
        return boost::lockfree::atomic_cas<compressed_ptr_t>::cas(&(this->ptr), oldval, newval);
    }

public:
    bool cas(tagged_ptr const & oldval, T * newptr)
    {
        compressed_ptr_t new_compressed_ptr = pack_ptr(newptr, extract_tag(oldval.ptr)+1);
        return cas(oldval.ptr, new_compressed_ptr);
    }

    bool cas(tagged_ptr const & oldval, T * newptr, tag_t t)
    {
        compressed_ptr_t new_compressed_ptr = pack_ptr(newptr, t);
        return boost::lockfree::atomic_cas<compressed_ptr_t>::cas(&(this->ptr), oldval.ptr, new_compressed_ptr);
    }
    /* @} */

    /** smart pointer support  */
    /* @{ */
    T & operator*() const
    {
        return *get_ptr();
    }

    T * operator->() const
    {
        return get_ptr();
    }

    operator bool(void) const
    {
        return get_ptr() != 0;
    }
    /* @} */

protected:
    compressed_ptr_t ptr;
};
#else
#error unsupported platform
#endif

} /* namespace lockfree */
} /* namespace boost */

#endif /* BOOST_LOCKFREE_TAGGED_PTR_PTRCOMPRESSION_HPP_INCLUDED */
