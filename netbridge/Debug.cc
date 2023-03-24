#include "Debug.h"

namespace nb {

namespace internal {

std::shared_ptr<Logger> g_debug;
std::once_flag g_logInit;

void InitDebugLog(unsigned int level) {
  std::call_once(g_logInit, [level]() {
    g_debug =
        LogManager::Instance().CreateLog(level, logFile, "nb_debug_log");
  });
}

} // end namespace internal

} // namespace nb
