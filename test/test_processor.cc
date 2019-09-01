#include <test_processor.hh>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>

namespace kthread {
namespace  {

using namespace std;
using namespace testing;

TEST(test_processor, consruct)
{
  test_processor<int> p;
  EXPECT_TRUE(p.is_open());

  p.close();
  EXPECT_FALSE(p.is_open());
}

TEST(test_processor, process)
{
  test_processor<int> p;

  auto tx = p.tx();
  EXPECT_TRUE(tx.push(1));
  EXPECT_TRUE(tx.push(2));
  EXPECT_TRUE(tx.push(3));

  p.close();
  EXPECT_FALSE(tx.push(4));

  EXPECT_THAT(p.collect(), ElementsAre(1, 2, 3));
}

}
}
