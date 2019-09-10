#include <channel.hh>
#include <type_traits>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace kthread {
namespace  {

static_assert(std::is_copy_constructible<sender<int>>::value);
static_assert(std::is_move_constructible<sender<int>>::value);
static_assert(std::is_copy_assignable<sender<int>>::value);
static_assert(std::is_move_assignable<sender<int>>::value);

TEST(sender, forwards)
{
  auto q = make_queue<int>();
  sender<int> c(q);

  EXPECT_TRUE(c.send(1));
  EXPECT_EQ(1, q->pop().unwrap());

  q->close();
  EXPECT_FALSE(c.send(2));
}

TEST(channel, connected_pair)
{
  auto c = make_channel<int>();

  c.tx.send(1);
  c.tx.send(2);
  c.tx.send(3);

  EXPECT_EQ(1, c.rx.receive().unwrap());
  EXPECT_EQ(2, c.rx.receive().unwrap());
  EXPECT_EQ(3, c.rx.receive().unwrap());
}

TEST(receiver, foreach)
{
  auto c = make_channel<int>();

  c.tx.send(1);
  c.tx.send(2);
  c.tx.send(3);

  c.tx.close();

  std::vector<int> r;
  foreach(c.rx, [&r](auto i) { r.push_back(i); });

  EXPECT_THAT(r, testing::ElementsAre(1, 2, 3));
}

TEST(receiver, foreach_noncopyable)
{
  using T = std::unique_ptr<int>;

  auto c = make_channel<T>();

  c.tx.send(std::make_unique<int>(1));
  c.tx.send(std::make_unique<int>(2));
  c.tx.send(std::make_unique<int>(3));
  c.tx.close();

  std::vector<int> r;
  foreach(c.rx, [&r](auto&& i) { r.push_back(*i); });

  EXPECT_THAT(r, testing::ElementsAre(1, 2, 3));
}



}
}
