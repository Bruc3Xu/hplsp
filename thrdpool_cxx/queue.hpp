#ifndef QUEUE_H_
#define QUEUE_H_

#include <atomic>
#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>

template <typename T> class Queue final {

public:
  Queue(size_t maxlen) : msg_max(maxlen) {}
  void Put(const T &msg) {
    std::unique_lock<std::mutex> lk(put_mutex_);
    put_cond_.wait(lk,
                   [this]() { return put_head.size() < msg_max || nonblock; });
    put_head.push_back(msg);
    get_cond_.notify_one();
  }
  bool Get(T &msg) {

    std::lock_guard<std::mutex> lock(get_mutex_);
    if (!get_head.empty() || __msgqueue_swap() > 0) {
      msg = get_head.back();
      get_head.pop_back();
      return true;
    }
    return false;
  }
  void SetNonblock() {
    nonblock = true;
    std::lock_guard<std::mutex> lock(put_mutex_);
    get_cond_.notify_one();
    put_cond_.notify_all();
  }
  void SetBlock() { nonblock = false; }

private:
  size_t __msgqueue_swap() {
    std::unique_lock<std::mutex> lk(put_mutex_);
    get_cond_.wait(lk, [this]() { return put_head.size() > 0 || nonblock; });
    put_head.swap(get_head);
    put_cond_.notify_all();
    return get_head.size();
  }

private:
  size_t msg_max;

  std::atomic_bool nonblock{false};
  std::list<T> get_head;
  std::list<T> put_head;
  std::mutex get_mutex_;
  std::mutex put_mutex_;
  std::condition_variable get_cond_;
  std::condition_variable put_cond_;
};

#endif