#ifndef DEBUG_H
#define DEBUG_H

#include "Logger.h"
#include <mutex>

namespace nb {

namespace internal {

extern std::shared_ptr<Logger> g_debug;
void InitDebugLog(unsigned int level);

} // end namespace internal

} // namespace nb

#define NB_DEBUG LOG_DBGUG(nb::internal::g_debug)
#define NB_INFO LOG_INFO(nb::internal::g_debug)
#define NB_WARN LOG_WARN(nb::internal::g_debug)
#define NB_ERROR LOG_ERROR(nb::internal::g_debug)

#endif
