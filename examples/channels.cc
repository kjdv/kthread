#include <channel.hh>
#include <threadpool.hh>
#include <iostream>

namespace {

using namespace std;
using namespace kthread;

void consume(receiver<int> tx)
{
  while(tx.receive()
            .map([](auto&& item) {
              cout << "consuming " << item << '\n';
              return 0;
            })
            .is_some())
    ;
}

void produce(int n, sender<int> tx)
{
  for(int i = 0; i < n; ++i)
    tx.send(i);
  tx.close();
}

}

int main()
{
  threadpool pool;

  auto c = make_channel<int>();
  pool.post([=] { produce(10, c.tx); });
  pool.post([=] { consume(c.rx); });

  return 0;
}
