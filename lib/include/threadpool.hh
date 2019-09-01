#pragma once

#include "queue.hh"
#include <thread>
#include <vector>
#include <functional>

namespace kthread {

unsigned guess_num_cpu() noexcept;

class threadpool
{
public:
  using job_t = std::function<void()>;

  explicit threadpool(size_t num_threads = guess_num_cpu(),
                      size_t queue_size = queue<job_t>::default_max_size);

  ~threadpool();

  void close();

  bool is_open() const;

  bool post(job_t job);

private:
  void run();

  queue<job_t> d_q;
  std::vector<std::thread> d_ts;
  std::mutex d_close_mutex; // edge case: what if multiple threads want to close this?
};


}
