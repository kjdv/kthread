#pragma once

#include <queue.hh>
#include <memory>
#include <cassert>
#include <utility>

namespace kthread {

template <typename T>
using queue_ptr = std::shared_ptr<queue<T>>;

template <typename T, typename... Args>
queue_ptr<T> make_queue(Args&&... args)
{
  return std::make_shared<typename queue_ptr<T>::element_type>(std::forward<Args>(args)...);
}

template <typename T>
class sender
{
public:
  using value_type = T;

  explicit sender(queue_ptr<value_type> q);

  bool push(value_type value);

private:
  queue_ptr<value_type> d_q;
};

template <typename T>
class receiver
{
public:
  using value_type = T;
  using option = typename queue<value_type>::option;

  explicit receiver(queue_ptr<value_type> q);

  option pull();

private:
  queue_ptr<value_type> d_q;
};

template <typename T>
struct channel
{
  receiver<T> rx;
  sender<T> tx;
};

template <typename T>
channel<T> make_channel(size_t max_size = queue<T>::default_max_size);

template<typename T>
sender<T>::sender(queue_ptr<value_type> q)
  : d_q(std::move(q))
{
  assert(d_q);
}

template<typename T>
bool sender<T>::push(sender::value_type value)
{
  return d_q->push(std::move(value));
}

template<typename T>
receiver<T>::receiver(queue_ptr<value_type> q)
  : d_q(std::move(q))
{
  assert(d_q);
}

template<typename T>
typename receiver<T>::option receiver<T>::pull()
{
  return d_q->pop();
}

template <typename T>
channel<T> make_channel(size_t max_size)
{
  auto q = make_queue<T>(max_size);
  return channel<T>{receiver(q), sender(q)};
}

}
