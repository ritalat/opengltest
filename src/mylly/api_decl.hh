#pragma once

#if defined(_WIN32)
    #ifdef MYLLY_EXPORTS
        #define MYLLY_API __declspec(dllexport)
    #else
        #define MYLLY_API __declspec(dllimport)
    #endif
#else
    #define MYLLY_API __attribute__((visibility("default")))
#endif
