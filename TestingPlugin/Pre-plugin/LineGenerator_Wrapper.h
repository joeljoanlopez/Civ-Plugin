#pragma once

#include "LineGeneratorPlugin.h"


#if defined(_WIN32) || defined(_WIN64)
#  ifndef EXPORT_API
#    define EXPORT_API __declspec(dllexport)
#  endif
#else
#  ifndef EXPORT_API
#    define EXPORT_API
#  endif
#endif

extern "C" {
    EXPORT_API int LineGenerator_GenerateLine(
        float startX, float startY, float startZ,
        int count,
        float spacing,
        char direction,
        float* outArray
    );
}