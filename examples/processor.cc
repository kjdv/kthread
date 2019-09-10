#include <channel.hh>
#include <processor.hh>
#include <iostream>

namespace {

using namespace std;
using namespace kthread;

struct consumer
{
  void operator()(int i) const
  {
    cout << i << '\n';
  }
};

void fibonacci(int n, sender<int> tx)
{
  int a = 0;
  int b = 1;

  for(int i = 0; i < n; ++i)
  {
    tx.send(a);

    b = b + a;
    a = b - a;
  }
}

}

int main()
{
  processor<int, consumer> p;
  p.connect();

  fibonacci(10, p.tx());

  return 0;
}
