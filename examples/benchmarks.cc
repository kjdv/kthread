#include <benchmark/benchmark.h>
#include <queue.hh>
#include <thread>
#include <iostream>

namespace kthread {
namespace {

void bm_queue_push(benchmark::State& state)
{
  queue<int> q;
  std::thread t([&q] {
    while (q.pop().is_some())
      ;
  });

  for (auto _ : state)
    q.push(1);

  q.close();
  t.join();
}

BENCHMARK(bm_queue_push);

void bm_queue_pop(benchmark::State& state)
{
  queue<int> q;
  std::thread t([&q] {
    while (q.push(1))
      ;
  });

  for (auto _ : state)
    q.pop();

  q.close();
  t.join();
}

BENCHMARK(bm_queue_pop);

}
}

BENCHMARK_MAIN();