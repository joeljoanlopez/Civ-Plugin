// Wrapper file to include hex source files from StrategyMapGenerationPlugin

// Disable Unreal's preprocessor macros that might conflict with standard C++
#pragma push_macro("check")
#pragma push_macro("verify")
#undef check
#undef verify

#include "../../../../../../StrategyMapGenerationPlugin/src/hex/HexCoord.cpp"
#include "../../../../../../StrategyMapGenerationPlugin/src/hex/HexGrid.cpp"
#include "../../../../../../StrategyMapGenerationPlugin/src/hex/HexTile.cpp"

#pragma pop_macro("verify")
#pragma pop_macro("check")
