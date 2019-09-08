#pragma once

#include <results/option.hh>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <cassert>
#include <boost/circular_buffer.hpp>

namespace kthread
{

// basic synchronized queue
template <typename T>
class queue
{
public:
  using value_type = T;
  using option = results::option<value_type>;

  enum {
    // the default is a bit arbitrary. choosen at 1000 as that would in all but the occasional or pathological cases
    // result in a never full queue while also give decent protection against memory running out
    default_max_size = 1000
  };

  explicit queue(size_t max_size = default_max_size);
  ~queue();

  bool push(value_type item);

  bool try_push(value_type item);

  option pop();

  option try_pop();

  bool is_open() const;

  void close();

private:
  using lock_t = std::unique_lock<std::mutex>;

  bool not_empty() const;
  bool not_full() const;

  bool do_push(lock_t &l, value_type item);
  option do_pop(lock_t &l);

  size_t d_max_size;
  bool d_open{true};
  boost::circular_buffer<value_type> d_q;
  mutable std::mutex d_m;
  std::condition_variable d_notfull;
  std::condition_variable d_notempty;
};

template<typename T>
queue<T>::queue(size_t max_size)
  : d_max_size(max_size)
  , d_q(max_size)
{
  assert(d_max_size > 0);
}

template<typename T>
queue<T>::~queue()
{
  close();
}

template<typename T>
bool queue<T>::push(value_type item)
{
  lock_t l(d_m);
  d_notfull.wait(l, [this] { return not_full(); });

  return do_push(l, std::move(item));
}

template<typename T>
bool queue<T>::try_push(value_type item)
{
  lock_t l(d_m);

  if (!not_full())
    return false;

  return do_push(l, std::move(item));
}

template<typename T>
bool queue<T>::do_push(lock_t &l, value_type item)
{
  if (!d_open)
    return false;

  d_q.push_back(std::move(item));

  l.unlock();
  d_notempty.notify_one();

  return true;
}

template<typename T>
typename queue<T>::option queue<T>::pop()
{
  lock_t l(d_m);
  d_notempty.wait(l, [this] { return not_empty(); });

  return do_pop(l);
}

template<typename T>
typename queue<T>::option queue<T>::try_pop()
{
  lock_t l(d_m);
  if (!not_empty())
    return option::none();

  return do_pop(l);
}

template<typename T>
typename queue<T>::option queue<T>::do_pop(lock_t &l)
{
  if (d_q.empty()) // queue must have been closed
  {
    assert(!d_open);
    return option::none();
  }

  auto r = option::some(std::move(d_q.front()));
  d_q.pop_front();

  l.unlock();
  d_notfull.notify_one();

  return r;
}


template<typename T>
bool queue<T>::is_open() const
{
  lock_t l(d_m);
  return d_open;
}

template<typename T>
void queue<T>::close()
{
  lock_t l(d_m);
  d_open = false;

  l.unlock();
  d_notfull.notify_all();
  d_notempty.notify_all();
}

template<typename T>
bool queue<T>::not_empty() const
{
  return !d_open || !d_q.empty();
}

template<typename T>
bool queue<T>::not_full() const
{
  return !d_open || d_q.size() < d_max_size;
}

}
