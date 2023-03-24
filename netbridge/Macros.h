#include <mutex>

#define DECLARE_SINGLETON(class_name)                                        \
  class my_singleton {                                                       \
   public:                                                                   \
    static class_name *GetInstance() {                                       \
      static class_name *instance = nullptr;                                 \
      if (!instance) {                                                       \
        static std::once_flag flag;                                          \
        std::call_once(flag,                                                 \
                       [&] { instance = new (std::nothrow) class_name(); }); \
      }                                                                      \
      return instance;                                                       \
    }                                                                        \
                                                                             \
   private:                                                                  \
    class_name() = default;                                                  \
    class_name(const class_name &) = delete;                                 \
    class_name &operator=(const class_name &) = delete;                      \
  };