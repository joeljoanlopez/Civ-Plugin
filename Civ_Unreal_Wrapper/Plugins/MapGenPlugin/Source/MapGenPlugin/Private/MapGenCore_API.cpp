// Wrapper file to include MapGenerationAPI.cpp from StrategyMapGenerationPlugin

// Disable Unreal's preprocessor macros that might conflict with standard C++
#pragma push_macro("check")
#pragma push_macro("verify")
#undef check
#undef verify

#include "../../../../../../StrategyMapGenerationPlugin/src/api/MapGenerationAPI.cpp"

#pragma pop_macro("verify")
#pragma pop_macro("check")
