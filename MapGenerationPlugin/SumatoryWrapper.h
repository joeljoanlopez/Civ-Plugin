#pragma once

#include "SumatoryCorePlugin.h"

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
    EXPORT_API float Sumatory_CoreSum(float a, float b);
}