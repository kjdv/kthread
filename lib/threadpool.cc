#include <iostream>
#include <threadpool.hh>

namespace kthread {

threadpool::threadpool(size_t num_threads, size_t queue_size)
  : d_q(queue_size)
{
  assert(num_threads > 0);
  d_ts.reserve(num_threads);

  for(size_t i = 0; i < num_threads; ++i)
    d_ts.emplace_back([this] { run(); });
}

threadpool::~threadpool()
{
  close();
}

void threadpool::close()
{
  using lock_t = std::unique_lock<std::mutex>;

  lock_t l(d_close_mutex);

  d_q.close();

  for(auto&& t : d_ts)
    t.join();

  d_ts.clear();
}

bool threadpool::is_open() const
{
  return d_q.is_open();
}

bool threadpool::post(threadpool::job_t job)
{
  return d_q.push(std::move(job));
}

size_t threadpool::num_threads() const
{
  return d_ts.size();
}

void threadpool::run()
{
  foreach(d_q, [](auto&& job) { job(); });
}

unsigned guess_num_cpu() noexcept
{
  unsigned n = std::thread::hardware_concurrency();
  return n ? n : 1;
}

} // namespace kthread
