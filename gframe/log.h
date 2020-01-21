#if defined(LOGI)
#undef LOGI
#endif

#if defined(LOGE)
#undef LOGE
#endif

#ifdef __ANDROID__
#include <android/log.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_DEBUG, "Edopro", __VA_ARGS__);
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "Edopro", __VA_ARGS__);
#else
#define LOGI(...)
#define LOGE(...)
#endif
