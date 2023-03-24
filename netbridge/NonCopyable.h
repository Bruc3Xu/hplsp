#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

namespace interval {
class noncopyable {
protected:
#if __cplusplus >= 201103L
  noncopyable() = default;
  ~noncopyable() = default;
#else
  noncopyable() {}
  ~noncopyable() {}
#endif
#if __cplusplus >= 201103L
  noncopyable(const noncopyable &) = delete;
  noncopyable &operator=(const noncopyable &) = delete;
#else
private: // emphasize the following members are private
  noncopyable(const noncopyable &);
  noncopyable &operator=(const noncopyable &);
#endif
};
} // namespace interval

#endif // NONCOPYABLE_H