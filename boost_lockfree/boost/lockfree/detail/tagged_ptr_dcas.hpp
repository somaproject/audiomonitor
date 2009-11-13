//  tagged pointer, for aba prevention
//
//  Copyright (C) 2008 Tim Blechmann
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  Disclaimer: Not a Boost library.

#ifndef BOOST_LOCKFREE_TAGGED_PTR_DCAS_HPP_INCLUDED
#define BOOST_LOCKFREE_TAGGED_PTR_DCAS_HPP_INCLUDED

#include <boost/lockfree/detail/cas.hpp>
#include <boost/lockfree/detail/branch_hints.hpp>

#include <cstddef>              /* for std::size_t */

namespace boost
{
namespace lockfree
{

template <class T>
class BOOST_LOCKFREE_DCAS_ALIGNMENT tagged_ptr
{
public:
    typedef std::size_t tag_t;

    static const bool is_lockfree = boost::lockfree::atomic_cas<tagged_ptr>::is_lockfree;

    /** uninitialized constructor */
    tagged_ptr(void)//: ptr(0), tag(0)
    {}

    /** copy constructor */
    tagged_ptr(tagged_ptr const & p)//: ptr(0), tag(0)
    {
        set(p);
    }

    explicit tagged_ptr(T * p, tag_t t = 0):
        ptr(p), tag(t)
    {}

    /** atomic set operations */
    /* @{ */
    void operator= (tagged_ptr const & p)
    {
        set(p);
    }

    void atomic_set(tagged_ptr const & p)
    {
        for (;;)
        {
            tagged_ptr old;
            old.set(*this);
            if(likely(cas(old, p.ptr, p.tag)))
                return;
        }
    }

    void atomic_set(T * p, tag_t t)
    {
        for (;;)
        {
            tagged_ptr old;
            old.set(*this);

            if(likely(cas(old, p, t)))
                return;
        }
    }
    /* @} */

    /** unsafe set operation */
    /* @{ */
    void set(tagged_ptr const & p)
    {
        ptr = p.ptr;
        tag = p.tag;
    }

    void set(T * p, tag_t t)
    {
        ptr = p;
        tag = t;
    }
    /* @} */

    /** comparing semantics */
    /* @{ */
    bool operator== (tagged_ptr const & p) const
    {
        return (ptr == p.ptr) && (tag == p.tag);
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
        return ptr;
    }

    void set_ptr(T * p)
    {
        ptr = p;
    }
    /* @} */

    /** tag access */
    /* @{ */
    tag_t get_tag() const
    {
        return tag;
    }

    void set_tag(tag_t t)
    {
        tag = t;
    }
    /* @} */

    /** compare and swap  */
    /* @{ */
    bool cas(tagged_ptr const & oldval, T * newptr)
    {
        return cas(oldval, newptr, oldval.tag + 1);
    }

    bool cas(tagged_ptr const & oldval, T * newptr, tag_t t)
    {
        tagged_ptr newval(newptr, t);
        return boost::lockfree::atomic_cas<tagged_ptr>::cas(this, oldval, newval);
    }
    /* @} */

    /** smart pointer support  */
    /* @{ */
    T & operator*() const
    {
        return *ptr;
    }

    T * operator->() const
    {
        return ptr;
    }

    operator bool(void) const
    {
        return ptr != 0;
    }
    /* @} */

protected:
    T * ptr;
    tag_t tag;
};

} /* namespace lockfree */
} /* namespace boost */

#endif /* BOOST_LOCKFREE_TAGGED_PTR_DCAS_HPP_INCLUDED */
