#include <boost/lockfree/detail/cas.hpp>
#include <boost/cstdint.hpp>

#include <climits>
#define BOOST_TEST_MODULE lockfree_tests
#include <boost/test/included/unit_test.hpp>

#include <cstddef>

using namespace boost::lockfree;

template <typename int_type>
void run_test(void)
{
    const int_type i = 42;
    const int_type j = ~42;

    int_type t1 = i;
    BOOST_REQUIRE_EQUAL (cas(&t1, i, j), true);
    BOOST_REQUIRE_EQUAL (t1, j);

    int_type t2 = i;
    BOOST_REQUIRE_EQUAL (cas(&t2, int_type(i-1), j), false);
    BOOST_REQUIRE_EQUAL (t2, i);
};


BOOST_AUTO_TEST_CASE( cas_test )
{
    run_test<boost::int16_t>();
    run_test<boost::uint16_t>();
    run_test<boost::int32_t>();
    run_test<boost::uint32_t>();
#ifndef BOOST_NO_INT64_T
    run_test<boost::int64_t>();
    run_test<boost::uint64_t>();
#endif
}

struct cas2_type
{
    cas2_type(long a, long b):
        a(a), b(b)
    {}

    long a;
    long b;
};

BOOST_AUTO_TEST_CASE( cas2_test )
{
    cas2_type ref(3, 5);
    cas2_type orig(3, 5);
    cas2_type next(4, 6);

    BOOST_REQUIRE_EQUAL (cas(&ref, orig, next), true);
    BOOST_REQUIRE_EQUAL (ref.a, next.a);
    BOOST_REQUIRE_EQUAL (ref.b, next.b);
}