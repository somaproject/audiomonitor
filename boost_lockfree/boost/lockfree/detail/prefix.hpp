//  Copyright (C) 2007, 2008, 2009 Tim Blechmann & Thomas Grill
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  Disclaimer: Not a Boost library.

#ifndef BOOST_LOCKFREE_PREFIX_HPP_INCLUDED
#define BOOST_LOCKFREE_PREFIX_HPP_INCLUDED

#include <cassert>


#ifdef _WIN32
    #include <windows.h>
#endif

#ifdef __APPLE__
    #include <libkern/OSAtomic.h>
#endif

#define BOOST_LOCKFREE_CACHELINE_BYTES 64

#ifdef _MSC_VER
// \note: Must use /Oi option for VC++ to enable intrinsics
    extern "C" {
        void __cdecl _ReadWriteBarrier();
        LONG __cdecl _InterlockedCompareExchange(LONG volatile* Dest,LONG Exchange, LONG Comp);
    }

#define BOOST_LOCKFREE_CACHELINE_ALIGNMENT __declspec(align(BOOST_LOCKFREE_CACHELINE_BYTES))

#if defined(_M_IX86)
    #define BOOST_LOCKFREE_DCAS_ALIGNMENT
#elif defined(_M_X64) || defined(_M_IA64)
    #define BOOST_LOCKFREE_PTR_COMPRESSION 1
    #define BOOST_LOCKFREE_DCAS_ALIGNMENT __declspec(align(16))
#endif

#endif /* _MSC_VER */

#ifdef __GNUC__

#define BOOST_LOCKFREE_CACHELINE_ALIGNMENT __attribute__((aligned(BOOST_LOCKFREE_CACHELINE_BYTES)))

#ifdef __i386__
    #define BOOST_LOCKFREE_DCAS_ALIGNMENT
#elif defined(__ppc__)
    #define BOOST_LOCKFREE_DCAS_ALIGNMENT
#elif defined(__x86_64__)

    #if !(defined (__GCC_HAVE_SYNC_COMPARE_AND_SWAP_16))
        #define BOOST_LOCKFREE_PTR_COMPRESSION 1
    #endif
    #define BOOST_LOCKFREE_DCAS_ALIGNMENT __attribute__((aligned(16)))
#endif


#if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 2))
#include <ext/atomicity.h>
#else
#include <bits/atomicity.h>
#endif


#endif /* __GNUC__ */


#ifdef USE_ATOMIC_OPS
    #define AO_REQUIRE_CAS
    #define AO_USE_PENTIUM4_INSTRS

    extern "C" {
        #include "../libatomic_ops/src/atomic_ops.h"
    }
#endif


#endif /* BOOST_LOCKFREE_PREFIX_HPP_INCLUDED */
