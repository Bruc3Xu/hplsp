#ifndef CHANNEL_H_
#define CHANNEL_H_
#include <functional>
#include <memory>

class EventLoop;

class Channel {
public:
  using EventCallback = std::function<void()>;
  using ReadEventCallback = std::function<void(int64_t)>;
  Channel();
  void SetReadCallback(ReadEventCallback cb) { read_callback_ = std::move(cb); }
  void SetWriteCallback(EventCallback cb) { write_callback_ = std::move(cb); }
  void SetErrorCallback(EventCallback cb) { error_callback_ = std::move(cb); }
  void SetCloseCallback(EventCallback cb) { close_callback_ = std::move(cb); }
  void SetREvents(int revents){revents_ = revents;}
  void HandleEvent();

private:
  int fd_;
  int events_;
  int revents_;
  std::unique_ptr<EventLoop> loop_;
  ReadEventCallback read_callback_;
  EventCallback write_callback_;
  EventCallback error_callback_;
  EventCallback close_callback_;
};

#endif