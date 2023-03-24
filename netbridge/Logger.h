#ifndef LOGGER_H
#define LOGGER_H

///@file Logger.h
///@brief A multi-thread Logger class

#include <atomic>
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include "Buffer.h"
#include "MmapFile.h"

enum LogLevel {
  logINFO = 0x01 << 0,
  logDEBUG = 0x01 << 1,
  logWARN = 0x01 << 2,
  logERROR = 0x01 << 3,
  logALL = 0xFFFFFFFF,
};

enum LogDest {
  logConsole = 0x01 << 0,
  logFile = 0x01 << 1,
  logSocket = 0x01 << 2,
};

namespace nb {

class Logger {
public:
  friend class LogManager;

  Logger();
  ~Logger();

  Logger(const Logger &) = delete;
  void operator=(const Logger &) = delete;
  Logger(Logger &&) = delete;
  void operator=(Logger &&) = delete;

  bool Init(unsigned int level = logDEBUG, unsigned int dest = logConsole,
            const char *pDir = 0);

  void Flush(LogLevel level);
  bool IsLevelForbid(unsigned int level) const { return !(level & level_); };

  Logger &operator<<(const char *msg);
  Logger &operator<<(const unsigned char *msg);
  Logger &operator<<(const std::string &msg);
  Logger &operator<<(void *);
  Logger &operator<<(unsigned char a);
  Logger &operator<<(char a);
  Logger &operator<<(unsigned short a);
  Logger &operator<<(short a);
  Logger &operator<<(unsigned int a);
  Logger &operator<<(int a);
  Logger &operator<<(unsigned long a);
  Logger &operator<<(long a);
  Logger &operator<<(unsigned long long a);
  Logger &operator<<(long long a);
  Logger &operator<<(double a);

  Logger &SetCurLevel(unsigned int level);

  void Shutdown();

  bool Update();

private:
  static const size_t kMaxCharPerLog = 2048;
  // parallel format log string
  static thread_local char tmpBuffer_[kMaxCharPerLog];
  static thread_local std::size_t pos_;
  static thread_local int64_t lastLogSecond_;
  static thread_local int64_t lastLogMSecond_;
  static thread_local unsigned int curLevel_;
  static thread_local char tid_[16];
  static thread_local int tidLen_;

  struct BufferInfo {
    std::atomic<bool> inuse_{false};
    Buffer buffer_;
  };

  std::mutex mutex_;
  std::map<std::thread::id, std::unique_ptr<BufferInfo>> buffers_;
  bool shutdown_;

  // const vars from init()
  unsigned int level_;
  std::string directory_;
  unsigned int dest_;
  std::string fileName_;

  internal::OMmapFile file_;

  std::size_t _Log(const char *data, std::size_t len);

  bool _CheckChangeFile();
  const std::string &_MakeFileName();
  bool _OpenLogFile(const std::string &name);
  void _CloseLogFile();
  void _WriteLog(int level, std::size_t nLen, const char *data);
  void _Color(unsigned int color);
  void _Reset();

  static unsigned int seq_;
};

class LogManager {
public:
  static LogManager &Instance();

  void Start();
  void Stop();

  std::shared_ptr<Logger> CreateLog(unsigned int level, unsigned int dest,
                                    const char *dir = nullptr);

  void AddBusyLog(Logger *);
  Logger *NullLog() { return &nullLog_; }

private:
  LogManager();

  void Run();

  std::mutex logsMutex_;
  std::vector<std::shared_ptr<Logger>> logs_;

  std::mutex mutex_;
  std::condition_variable cond_;
  bool shutdown_;
  std::set<Logger *> busyLogs_;

  // null object
  Logger nullLog_;

  std::thread iothread_;
};

class LogHelper {
public:
  LogHelper(LogLevel level);
  Logger &operator=(Logger &log);

private:
  LogLevel level_;
};


#define LOG_DBGUG(x)                                                             \
  (!(x) || (x)->IsLevelForbid(logDEBUG))                                       \
      ? *nb::LogManager::Instance().NullLog()                              \
      : (nb::LogHelper(logDEBUG)) = x->SetCurLevel(logDEBUG)

#define LOG_INFO(x)                                                             \
  (!(x) || (x)->IsLevelForbid(logINFO))                                        \
      ? *nb::LogManager::Instance().NullLog()                              \
      : (nb::LogHelper(logINFO)) = x->SetCurLevel(logINFO)

#define LOG_WARN(x)                                                             \
  (!(x) || (x)->IsLevelForbid(logWARN))                                        \
      ? *nb::LogManager::Instance().NullLog()                              \
      : (nb::LogHelper(logWARN)) = x->SetCurLevel(logWARN)

#define LOG_ERROR(x)                                                             \
  (!(x) || (x)->IsLevelForbid(logERROR))                                       \
      ? *nb::LogManager::Instance().NullLog()                              \
      : (nb::LogHelper(logERROR)) = x->SetCurLevel(logERROR)

} // end namespace nb

#endif
