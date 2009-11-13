#include <climits>
#define BOOST_TEST_MODULE lockfree_tests
#include <boost/test/included/unit_test.hpp>

#include "test_helpers.hpp"

#include <boost/lockfree/stack.hpp>

#include <boost/thread.hpp>
using namespace boost;

template <typename freelist_t>
struct stack_tester
{
    static const unsigned int buckets = 1<<10;
    static_hashed_set<long, buckets> data;
    boost::array<std::set<long>, buckets> returned;

    boost::lockfree::stack<long, freelist_t> stk;

    void add_items(void)
    {
        unsigned long count = 100000;

        for (unsigned long i = 0; i != count; ++i)
        {
            thread::yield();
            long id = generate_id<long>();

            bool inserted = data.insert(id);

            assert(inserted);


            while(stk.push(id) == false)
            {
                thread::yield();
            }
        }
    }

    volatile bool running;

    void get_items(void)
    {
        for (;;)
        {
            thread::yield();
            long id;

            bool got = stk.pop(&id);
            if (got)
            {
                bool erased = data.erase(id);
                assert(erased);
            }
            else
                if (not running)
                    return;
        }
    }

    void run(void)
    {
        BOOST_CHECK((boost::lockfree::stack<long, freelist_t>::is_lockfree));

        running = true;

        thread_group writer;
        thread_group reader;

        BOOST_REQUIRE(stk.empty());

        for (int i = 0; i != 2; ++i)
            reader.create_thread(boost::bind(&stack_tester::get_items, this));

        for (int i = 0; i != 2; ++i)
            writer.create_thread(boost::bind(&stack_tester::add_items, this));

        using namespace std;
        cout << "threads created" << endl;

        writer.join_all();

        cout << "writer threads joined, waiting for readers" << endl;

        running = false;
        reader.join_all();

        cout << "reader threads joined" << endl;

        BOOST_REQUIRE_EQUAL(data.count_nodes(), 0);
        BOOST_REQUIRE(stk.empty());
    }
};


BOOST_AUTO_TEST_CASE( stack_test_caching )
{
    stack_tester<boost::lockfree::caching_freelist_t> tester;
    tester.run();
}

BOOST_AUTO_TEST_CASE( stack_test_static )
{
    stack_tester<boost::lockfree::static_freelist_t> tester;
    tester.run();
}
