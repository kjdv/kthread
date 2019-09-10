#include <processor.hh>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>

namespace kthread {
namespace  {

using namespace std;
using namespace testing;

struct consumer
{
  vector<int> &receiver;

  consumer(vector<int> &r)
    : receiver(r)
  {}

  void operator()(int i)
  {
    receiver.push_back(i);
  }
};

struct nc_consumer
{
  using type = std::unique_ptr<int>;

  vector<int> &receiver;

  nc_consumer(vector<int> &r)
    : receiver(r)
  {}

  void operator()(type&& i)
  {
    assert(i);
    receiver.push_back(*i);
  }
};

TEST(processor, consruct)
{
  processor<int, consumer> p;
  EXPECT_FALSE(p.is_open());

  vector<int> r;
  p.connect(r);
  EXPECT_TRUE(p.is_open());

  p.close();
  EXPECT_FALSE(p.is_open());
}

TEST(processor, process)
{
  processor<int, consumer> p;
  vector<int> r;
  p.connect(r);

  auto tx = p.tx();
  EXPECT_TRUE(tx.send(1));
  EXPECT_TRUE(tx.send(2));
  EXPECT_TRUE(tx.send(3));

  p.close();
  EXPECT_FALSE(tx.send(4));

  EXPECT_THAT(r, ElementsAre(1, 2, 3));
}

TEST(processor, noncopyable)
{
  using T = std::unique_ptr<int>;
  auto make = [](int i) { return std::make_unique<int>(i); };

  vector<int> r;
  processor<T, nc_consumer> p;
  p.connect(r);

  auto tx = p.tx();
  tx.send(make(1));
  tx.send(make(2));
  tx.send(make(3));

  p.close();

  EXPECT_THAT(r, ElementsAre(1, 2, 3));
}

}
}
