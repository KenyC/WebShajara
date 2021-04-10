#ifdef DEBUG

#ifdef WEB
#include <emscripten.h>
#define LOG(...) emscripten_log(EM_LOG_CONSOLE, __VA_ARGS__) 
#else
#include <stdio.h>
#define LOG(...) printf(__VA_ARGS__) 
#endif

#else
#define LOG(...) 
#endif
