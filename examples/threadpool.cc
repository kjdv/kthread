#include <threadpool.hh>
#include <iostream>

using namespace std;
using namespace kthread;

namespace {

void hello(int n)
{
  static mutex m;
  using lock_t = unique_lock<mutex>;

  lock_t l(m);

  cout << "hello number " << n << " from thread " << this_thread::get_id() << "\n";
}

}

int main()
{
  threadpool pool;

  for (int i = 0; i < 100; ++i)
    pool.post([=] { hello(i); });

  return 0;
}
