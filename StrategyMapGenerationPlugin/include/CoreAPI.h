#pragma once

#if defined(_WIN32)
    #if defined(MAPGEN_EXPORTS)
        #define MAPGEN_API __declspec(dllexport)
    #else
        #define MAPGEN_API __declspec(dllimport)
    #endif
#else
    #define MAPGEN_API __attribute__((visibility("default")))
#endif