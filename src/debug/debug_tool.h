#ifndef DEBUG_TOOL_H_
#define DEBUG_TOOL_H_

#if !defined(SENTRY_DEBUG_ENABLE)
#define SENTRY_DEBUG_ENABLE      0
#endif

#if SENTRY_DEBUG_ENABLE != 0
#if !defined(ERROR_OUTPUT)
#define ERROR_OUTPUT            1
#endif
#if !defined(WARNING_OUTPUT)
#define WARNING_OUTPUT          1
#endif
// LOG_OUTPUT:  1 -> Simple Output
//              2 -> Complete Output
#if !defined(LOG_OUTPUT)
#define LOG_OUTPUT              1
#endif
#endif  /* SENTRY_DEBUG_ENABLE != 0 */

#if LOG_OUTPUT != 0
#define IPRINTF(s,...)                  printf("\e[0;32mI:" s "\e[0m", ##__VA_ARGS__)
#else
#define IPRINTF(s,...)
#endif

#if WARNING_OUTPUT != 0
#define WPRINTF(s,...)                  printf("\e[0;33min file:%s, function:%s, line: %d\nW:" s "\e[0m",\
                                                __FILE__, \
                                                __PRETTY_FUNCTION__,\
                                                __LINE__, \
                                                ##__VA_ARGS__)
#else
#define WPRINTF(s,...)
#endif

#if ERROR_OUTPUT != 0
#define EPRINTF(s,...)                  printf("\e[0;31min file:%s, function:%s, line: %d\nE:" s "\e[0m",\
                                                __FILE__,\
                                                __PRETTY_FUNCTION__,\
                                                __LINE__,\
                                                ##__VA_ARGS__)
#else
#define EPRINTF(s,...)
#endif

#endif /* DEBUG_TOOL_H_ */
