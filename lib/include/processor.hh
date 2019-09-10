#pragma once

#include "channel.hh"
#include "threadpool.hh"

namespace kthread {

// model where only the processor manages a thread, and the consumer lives completely isolated.
// One can only communicate with the consumer through a connected channel, no one else can even have a reference to it
template <typename T, typename C>
class processor
{
public:
  explicit processor(size_t max_size = queue<T>::default_max_size);

  ~processor();

  bool is_open() const;

  void close();

  // connects the processor to a consumer. This is done in place to make it hard (not impossible, you can still cheat)
  // to get a reference to it other than through the sending channel
  template<typename... Args>
  void connect(Args&&... args);

  sender<T> tx() const;

private:
  void run();

  channel<T> d_channel;
  threadpool d_thread;
  std::optional<C> d_consumer;
};

template <typename T, typename C>
processor<T, C>::processor(size_t max_size)
  : d_channel(make_channel<T>(max_size))
  , d_thread(1, 1)
{}

template <typename T, typename C>
processor<T, C>::~processor()
{
  close();
}

template <typename T, typename C>
bool processor<T, C>::is_open() const
{
  return d_consumer.has_value() && d_thread.is_open();
}

template <typename T, typename C>
void processor<T, C>::close()
{
  d_channel.tx.close();
  d_thread.close();
}

template <typename T, typename C>
template <typename... Args>
void processor<T, C>::connect(Args&&... args)
{
  assert(!d_consumer && "can't call connect() twice");

  d_consumer.emplace(std::forward<Args>(args)...);
  d_thread.post([this] { run(); });
}

template <typename T, typename C>
sender<T> processor<T, C>::tx() const
{
  return d_channel.tx;
}

template <typename T, typename C>
void processor<T, C>::run()
{
  assert(d_consumer);

  foreach(d_channel.rx, [this](auto&& item) { (*d_consumer)(std::move(item)); });
}

}
