#include <channel.hh>
#include <type_traits>
#include <gtest/gtest.h>

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

  EXPECT_TRUE(c.push(1));
  EXPECT_EQ(1, q->pop().unwrap());

  q->close();
  EXPECT_FALSE(c.push(2));
}

TEST(channel, connected_pair)
{
  auto c = make_channel<int>();

  c.tx.push(1);
  c.tx.push(2);
  c.tx.push(3);

  EXPECT_EQ(1, c.rx.pull().unwrap());
  EXPECT_EQ(2, c.rx.pull().unwrap());
  EXPECT_EQ(3, c.rx.pull().unwrap());
}

}
}
