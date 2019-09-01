#include <queue.hh>
#include <gtest/gtest.h>
#include <type_traits>
#include <thread>
#include <vector>
#include <set>

namespace kthread {
namespace {

static_assert(!std::is_copy_constructible_v<queue<int>>);
static_assert(!std::is_copy_assignable_v<queue<int>>);
static_assert(!std::is_move_constructible_v<queue<int>>);
static_assert(!std::is_move_assignable_v<queue<int>>);

TEST(queue, construct)
{
  queue<int> q;
  EXPECT_TRUE(q.is_open());

  q.close();
  EXPECT_FALSE(q.is_open());
}

TEST(queue, st_push_pop)
{
  queue<int> q(3);
  q.push(1);
  q.push(2);
  q.push(3);

  EXPECT_EQ(1, q.pop().unwrap());
  EXPECT_EQ(2, q.pop().unwrap());
  EXPECT_EQ(3, q.pop().unwrap());
}

TEST(queue, try_push_pop)
{
  queue<int> q(3);
  EXPECT_TRUE(q.try_push(1));
  EXPECT_TRUE(q.try_push(2));
  EXPECT_TRUE(q.try_push(3));

  EXPECT_FALSE(q.try_push(4));

  EXPECT_EQ(1, q.pop().unwrap());
  EXPECT_EQ(2, q.pop().unwrap());
  EXPECT_EQ(3, q.pop().unwrap());

  EXPECT_TRUE(q.try_push(4));
  EXPECT_EQ(4, q.pop().unwrap());
}

TEST(queue, push_try_pop)
{
  queue<int> q;

  EXPECT_TRUE(q.try_pop().is_none());

  q.push(1);
  EXPECT_EQ(1, q.try_pop().unwrap());
  EXPECT_TRUE(q.try_pop().is_none());
}

TEST(queue, close_results_in_nones)
{
  queue<int> q;
  EXPECT_TRUE(q.push(1));

  q.close();
  EXPECT_EQ(1, q.pop().unwrap());
  EXPECT_TRUE(q.pop().is_none());

  EXPECT_FALSE(q.push(2));
}

TEST(queue, multi_producer_multi_consumer)
{
  queue<int> q(5);

  // producers
  std::vector<std::thread> producers;
  for (unsigned i = 0; i < 10; ++i)
  {
    producers.emplace_back([=, &q] {
      for (int j = 0; j < 10; ++j)
        q.push(i * 10 + j);
    });
  }

  // consumers
  queue<int> sink(1000);
  std::vector<std::thread> consumers;
  for (unsigned i = 0; i < 10; ++i)
  {
    consumers.emplace_back([&] {
      while (q.pop()
              .map([&](auto&& item) { sink.push(item); return 0; })
              .is_some())
        ;
    });
  }

  // collect
  for (auto&&t : producers)
    t.join();
  q.close();
  for (auto&&t : consumers)
    t.join();
  sink.close();

  std::set<int> found;
  while (sink.pop()
             .map([&](auto&& item) { found.insert(item); return 0; })
             .is_some())
    ;

  // check
  EXPECT_EQ(100, found.size());
  EXPECT_EQ(0, *found.begin());
  EXPECT_EQ(99, *found.rbegin());
}

}
}
