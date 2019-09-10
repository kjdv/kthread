#pragma once

#include <channel.hh>
#include <vector>

namespace kthread {

// variant of a procesor that does not start a thread and just acts as a sink for messages send on the channel
// this is mostly useful for unit tests that don't need an actual thread or consumer, but just need to check if what
// is send on the channel matches your expectations
template <typename T>
class test_processor
{
public:
  explicit test_processor(size_t max_size = queue<T>::default_max_size);

  bool is_open() const;

  void close();

  sender<T> tx() const;

  std::vector<T> collect();

private:
  queue_ptr<T> d_q;
};

template<typename T>
test_processor<T>::test_processor(size_t max_size)
  : d_q(make_queue<T>(max_size))
{}

template<typename T>
bool test_processor<T>::is_open() const
{
  return d_q->is_open();
}

template<typename T>
void test_processor<T>::close()
{
  d_q->close();
}

template<typename T>
sender<T> test_processor<T>::tx() const
{
  return sender<T>(d_q);
}

template<typename T>
std::vector<T> test_processor<T>::collect()
{
  std::vector<T> r;
  while(d_q->try_pop()
            .map([&](auto && item) { r.push_back(item); })
            .is_some())
    ;
  return r;
}

}
