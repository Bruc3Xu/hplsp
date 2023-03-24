#include <assert.h>
#include <string.h>
#include <string>

class Slice {
public:
  Slice() : data_(""), size_(0) {}
  Slice(const char *data, size_t size) : data_(data), size_(size) {}
  Slice(const char *data) : data_(data), size_(strlen(data)) {}
  Slice(const std::string &data) : data_(data.c_str()), size_(data.length()) {}

  Slice(const Slice &) = default;
  Slice &operator=(const Slice &) = default;

  const char *data() const { return data_; }
  size_t size() const { return size_; }
  bool empty() const { return size_ == 0; }
  char operator[](size_t n) const {
    assert(n < size_);
    return data_[n];
  }
  void clear() {
    data_ = "";
    size_ = 0;
  }

  void remove_prefix(size_t n) {
    assert(n <= size_);
    data_ += n;
    size_ -= n;
  }

  std::string ToString() const { return std::string(data_, size_); }

  bool starts_with(const Slice &s) const {
    return (size_ >= s.size_) && (memcmp(data_, s.data_, s.size_) == 0);
  }

  bool compare(const Slice &s) const;

private:
  const char *data_;
  size_t size_;
};

inline bool operator==(const Slice &x, const Slice &y) {
  return (x.size() == y.size()) && (memcmp(x.data(), y.data(), x.size()) == 0);
}

inline bool Slice::compare(const Slice &s) const {
  size_t min_len = (size_ > s.size_) ? size_ : s.size_;
  int r = memcmp(data_, s.data_, min_len);
  if (r == 0) {
    if (size_ < s.size_) {
      r = -1;
    } else if (size_ > s.size_) {
      r = 1;
    }
  }
  return r;
}