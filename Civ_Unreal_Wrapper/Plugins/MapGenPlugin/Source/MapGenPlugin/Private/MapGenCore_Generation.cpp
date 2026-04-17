// Wrapper file to include generation source files from StrategyMapGenerationPlugin

// Disable Unreal's preprocessor macros that might conflict with standard C++
#pragma push_macro("check")
#pragma push_macro("verify")
#undef check
#undef verify

// Match floating-point semantics of the CMake build
#ifdef _MSC_VER
    #pragma float_control(precise, on, push)
#else
    #pragma STDC FP_CONTRACT OFF
#endif

#include "../../../../../../StrategyMapGenerationPlugin/src/generation/PerlinNoiseGenerator.cpp"
#include "../../../../../../StrategyMapGenerationPlugin/src/generation/RandomGenerator.cpp"
#include "../../../../../../StrategyMapGenerationPlugin/src/generation/TectonicsGenerator.cpp"

#ifdef _MSC_VER
    #pragma float_control(pop)
#endif

#pragma pop_macro("verify")
#pragma pop_macro("check")
