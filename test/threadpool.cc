#include <threadpool.hh>
#include <gtest/gtest.h>
#include <type_traits>
#include <set>
#include <iostream>
#include <atomic>

namespace kthread {
namespace {

static_assert(!std::is_copy_constructible_v<threadpool>);
static_assert(!std::is_copy_assignable_v<threadpool>);
static_assert(!std::is_move_constructible_v<threadpool>);
static_assert(!std::is_move_assignable_v<threadpool>);

class fixture
{
public:
  fixture(size_t num_threads)
    : d_q(std::in_place)
    , d_pool(num_threads)
  {}

  void close()
  {
    d_pool.close();
  }

  bool job(int min, int max)
  {
    ++d_running;

    bool r = d_pool.post([=]{
      for(int i = min; i < max; ++i)
      {
        auto r = d_q.value().push(i);
        assert(r);
      }
      --d_running;
    });

    if(!r)
      --d_running;

    return r;
  }

  std::set<int> collect()
  {
    while(d_running > 0)
      std::this_thread::yield();

    d_q.value().close();

    std::set<int> r;
    while(true)
    {
      auto item = d_q.value().pop();
      if (item.is_none())
        break;
      r.insert(item.unwrap());
    }

    d_q.emplace();

    return r;
  }

private:
  std::optional<queue<int>> d_q;
  threadpool d_pool;
  std::atomic<int> d_running{0};
};

TEST(threadpool, construct)
{
  threadpool t;
  EXPECT_TRUE(t.is_open());

  t.close();
  EXPECT_FALSE(t.is_open());
}

TEST(threadpool, execute)
{
  fixture f(4);

  for(int i = 0; i < 100; i += 10)
    f.job(i, i + 10);

  auto s = f.collect();

  EXPECT_EQ(100, s.size());
  EXPECT_EQ(0, *s.begin());
  EXPECT_EQ(99, *s.rbegin());
}

TEST(threadpool, close)
{
  fixture f(4);

  EXPECT_TRUE(f.job(0, 1));

  f.close();

  EXPECT_FALSE(f.job(1, 2));

  auto s = f.collect();

  EXPECT_EQ(1, s.size());
  EXPECT_EQ(0, *s.begin());
}

TEST(threadpool, multiple_close)
{
  threadpool t(4);

  std::vector<std::thread> threads;
  for(unsigned i = 0; i < 10; ++i)
    threads.emplace_back([&] { t.close(); });

  for (auto&& t : threads)
    t.join();
}

}
}
