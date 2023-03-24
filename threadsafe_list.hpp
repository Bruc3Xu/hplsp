#include <algorithm>
#include <memory>
#include <mutex>

template <typename T> class threadsafe_list {
private:
  struct node {
    std::mutex m;
    std::shared_ptr<T> data;
    std::unique_ptr<node> next;
    node() : next() {}
    node(T const &value) : data(std::make_shared<T>(value)), next() {}
  };
  node head;

public:
  threadsafe_list() {}
  ~threadsafe_list() {
    std::remove_if([](node const &) { return true; });
  }
  threadsafe_list(threadsafe_list const &other) = delete;
  threadsafe_list &operator=(threadsafe_list const &other) = delete;
  void push_front(T const &value) {
    std::unique_ptr<node> new_node(new node(value));
    std::lock_guard<std::mutex> lock(head.m);
    new_node->next = std::move(head.next);
    head.next = std::move(new_node);
  }
  template <typename Function> void for_each(Function f) {
    node *current = &head;
    std::unique_lock<std::mutex> lock(head.m);
    while (node *const next = current->next().get()) {
      std::unique_lock<std::mutex> next_lk(next->m);
      lock.unlock();
      f(*next->data);
      current = next;
      lock = std::move(next_lk);
    }
  }

  template <typename Predicate> std::shared_ptr<T> find_first_if(Predicate P) {
    node *current = &head;
    std::unique_lock<std::mutex> lock(head.m);
    while (node *const next = current->next.get()) {
      std::unique_lock<std::mutex> next_lk(next->m);
      lock.unlock();
      if (P(*next->data)) {
        return next->data;
      }
      current = next;
      lock = std::move(next_lk);
    }
    return std::shared_ptr<T>();
  }
  template <typename Predicate> void remove_if(Predicate p) {
    node *current = &head;
    std::unique_lock<std::mutex> lk(head.m);
    while (node *const next = current->next.get()) {
      std::unique_lock<std::mutex> next_lk(next->m);
      if (p(*next->data)) {
        std::unique_ptr<node> old_next = std::move(current->next);
        current->next = std::move(next->next);
        next_lk.unlock();
      } else {
        lk.unlock();
        current = next;
        lk = std::move(next_lk);
      }
    }
  }
};