#include <boost/lockfree/detail/freelist.hpp>

#include <climits>
#define BOOST_TEST_MODULE lockfree_tests
#include <boost/test/included/unit_test.hpp>

#include <boost/foreach.hpp>
#include <boost/static_assert.hpp>

#include <boost/type_traits/is_same.hpp>

#include <vector>

class dummy
{
    int foo[64];
};

boost::lockfree::freelist<dummy, 64> fl;

BOOST_AUTO_TEST_CASE( freelist_test1 )
{
    std::vector<dummy*> nodes;

    for (int i = 0; i != 128; ++i)
        nodes.push_back(fl.allocate());

    BOOST_FOREACH(dummy * d, nodes)
        fl.deallocate(d);

    for (int i = 0; i != 128; ++i)
        nodes.push_back(fl.allocate());
}

boost::lockfree::caching_freelist<dummy> cfl;

BOOST_AUTO_TEST_CASE( freelist_test2 )
{
    std::vector<dummy*> nodes;

    for (int i = 0; i != 128; ++i)
        nodes.push_back(cfl.allocate());

    BOOST_FOREACH(dummy * d, nodes)
        cfl.deallocate(d);

    for (int i = 0; i != 128; ++i)
        nodes.push_back(cfl.allocate());
}

boost::lockfree::static_freelist<dummy> sfl(128);

BOOST_AUTO_TEST_CASE( freelist_test3 )
{
    std::vector<dummy*> nodes;

    for (int i = 0; i != 128; ++i)
        nodes.push_back(sfl.allocate());

    BOOST_FOREACH(dummy * d, nodes)
        sfl.deallocate(d);

    for (int i = 0; i != 128; ++i)
        nodes.push_back(sfl.allocate());
}

/* using namespace boost; */
/* using namespace boost::mpl; */

/* BOOST_STATIC_ASSERT((is_same<lockfree::detail::select_freelist<int, std::allocator<int>, lockfree::caching_freelist_t>, */
/*                      lockfree::caching_freelist<int, std::allocator<int> > */
/*                      >::value)); */

/* BOOST_STATIC_ASSERT((is_same<lockfree::detail::select_freelist<int, std::allocator<int>, lockfree::static_freelist_t>, */
/*                      lockfree::static_freelist<int, std::allocator<int> > */
/*                      >::value)); */
