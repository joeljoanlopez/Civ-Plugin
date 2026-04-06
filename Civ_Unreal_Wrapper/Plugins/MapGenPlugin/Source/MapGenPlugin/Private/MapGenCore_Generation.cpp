// Wrapper file to include generation source files from StrategyMapGenerationPlugin

// Disable Unreal's preprocessor macros that might conflict with standard C++
#pragma push_macro("check")
#pragma push_macro("verify")
#undef check
#undef verify

#include "../../../../../../StrategyMapGenerationPlugin/src/generation/PerlinNoiseGenerator.cpp"
#include "../../../../../../StrategyMapGenerationPlugin/src/generation/RandomGenerator.cpp"
#include "../../../../../../StrategyMapGenerationPlugin/src/generation/TectonicsGenerator.cpp"

#pragma pop_macro("verify")
#pragma pop_macro("check")
