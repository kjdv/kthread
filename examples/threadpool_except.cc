#include <threadpool.hh>
#include <iostream>
#include <exception>

using namespace std;
using namespace kthread;

int main()
{
  set_terminate([]{
    cout << "unhandled exception, exiting with exit code 0 for demo purposes\n";
    exit(0);
  });

  threadpool pool;

  // throwing an exception in a job will cause std::terminate to be called
  pool.post([]{ throw std::runtime_error("booh!"); });

  return 1; // terminate() not called
}
