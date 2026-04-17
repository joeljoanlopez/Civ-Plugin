// Wrapper file to include MapGenerationAPI.cpp from StrategyMapGenerationPlugin

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

#include "../../../../../../StrategyMapGenerationPlugin/src/api/MapGenerationAPI.cpp"

#ifdef _MSC_VER
    #pragma float_control(pop)
#endif

#pragma pop_macro("verify")
#pragma pop_macro("check")
