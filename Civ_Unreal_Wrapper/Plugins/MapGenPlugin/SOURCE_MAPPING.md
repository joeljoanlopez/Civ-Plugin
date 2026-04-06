# Source File Mapping

This document shows how the Unreal plugin wrapper files map to the original StrategyMapGenerationPlugin source files.

## Wrapper Files (Unreal Plugin)

### Core Wrapper Implementation

| File | Purpose |
|------|---------|
| `Public/MapGeneratorWrapper.h` | Unreal Actor wrapper class with Blueprint support |
| `Private/MapGeneratorWrapper.cpp` | Implementation of wrapper, handles API calls and visualization |

### Dynamic Source Inclusion Files

| Wrapper File | Included Source Files |
|--------------|----------------------|
| `Private/MapGenCore_API.cpp` | `../../../../../StrategyMapGenerationPlugin/src/api/MapGenerationAPI.cpp` |
| `Private/MapGenCore_Generation.cpp` | `../../../../../StrategyMapGenerationPlugin/src/generation/PerlinNoiseGenerator.cpp`<br>`../../../../../StrategyMapGenerationPlugin/src/generation/RandomGenerator.cpp`<br>`../../../../../StrategyMapGenerationPlugin/src/generation/TectonicsGenerator.cpp` |
| `Private/MapGenCore_Hex.cpp` | `../../../../../StrategyMapGenerationPlugin/src/hex/HexCoord.cpp`<br>`../../../../../StrategyMapGenerationPlugin/src/hex/HexGrid.cpp`<br>`../../../../../StrategyMapGenerationPlugin/src/hex/HexTile.cpp` |

### Build Configuration

| File | Purpose |
|------|---------|
| `MapGenPlugin.Build.cs` | Configures include paths to StrategyMapGenerationPlugin headers |

## Referenced Source Files (StrategyMapGenerationPlugin)

### Headers (via PublicIncludePaths)
```
StrategyMapGenerationPlugin/include/
├── api/
│   └── MapGenerationAPI.h          # Main C API
├── core/
│   └── CoreAPI.h                    # Export macros
├── generation/
│   ├── PerlinNoiseGenerator.h
│   ├── RandomGenerator.h
│   └── TectonicsGenerator.h
└── hex/
    ├── HexCoord.h
    ├── HexGrid.h
    └── HexTile.h
```

### Source Files (via wrapper .cpp includes)
```
StrategyMapGenerationPlugin/src/
├── api/
│   └── MapGenerationAPI.cpp         # Included by MapGenCore_API.cpp
├── generation/
│   ├── PerlinNoiseGenerator.cpp     # Included by MapGenCore_Generation.cpp
│   ├── RandomGenerator.cpp          # Included by MapGenCore_Generation.cpp
│   └── TectonicsGenerator.cpp       # Included by MapGenCore_Generation.cpp
└── hex/
    ├── HexCoord.cpp                 # Included by MapGenCore_Hex.cpp
    ├── HexGrid.cpp                  # Included by MapGenCore_Hex.cpp
    └── HexTile.cpp                  # Included by MapGenCore_Hex.cpp
```

## Path Resolution

All paths are relative from the Unreal plugin source directory:
```
Civ_Unreal_Wrapper/Plugins/MapGenPlugin/Source/MapGenPlugin/Private/
```

To reach StrategyMapGenerationPlugin:
```
../../../../../StrategyMapGenerationPlugin/
```

Directory traversal:
1. `..` → MapGenPlugin (parent of Private)
2. `..` → Source (parent of MapGenPlugin)
3. `..` → Plugins (parent of Source)
4. `..` → MapGenPlugin (plugin root, parent of Plugins)
5. `..` → Civ_Unreal_Wrapper (parent of plugin)
6. `..` → Civ-Plugin (repository root)
7. `StrategyMapGenerationPlugin/` → target directory

## Build Configuration (MapGenPlugin.Build.cs)

```csharp
string MapGenCorePath = Path.GetFullPath(
    Path.Combine(ModuleDirectory, "../../../../../StrategyMapGenerationPlugin"));
string MapGenCoreIncludePath = Path.Combine(MapGenCorePath, "include");
string MapGenCoreSrcPath = Path.Combine(MapGenCorePath, "src");

PublicIncludePaths.AddRange(new string[] { MapGenCoreIncludePath });
PrivateIncludePaths.AddRange(new string[] { MapGenCoreSrcPath });
```

This configuration allows:
- Headers to be included as: `#include "api/MapGenerationAPI.h"`
- Source files to be included with full relative paths in wrapper files

## Type Mappings

| C API Type | Unreal Type | Notes |
|------------|-------------|-------|
| `MapGenTileData` (struct) | `FMapGenTileData` (USTRUCT) | Converted in MapGeneratorWrapper.cpp |
| `MapGenMapData` (struct) | Used directly | C struct used internally |
| `int` | `int32` | Unreal standard integer type |
| `float` | `float` | Direct mapping |
| `enum TerrainType` | `ETerrainType` (UENUM) | Blueprint-compatible enum |

## Function Calls

The wrapper provides a clean interface:

```cpp
// C API (from MapGenerationAPI.h)
int MapGenGenerateMap(int width, int height, int seed, int plateCount,
                      float landRatio, int noiseOctaves, MapGenMapData* outMap);
void MapGenFreeMap(MapGenMapData* mapData);

// Unreal Wrapper (in AMapGeneratorWrapper)
bool GenerateMap();  // Calls MapGenGenerateMap internally
void FreeCurrentMap(); // Calls MapGenFreeMap internally
```

## Memory Management

- C API allocates: `new MapGenTileData[totalCells]`
- Unreal copies to: `TArray<FMapGenTileData> Tiles`
- C API frees: `delete[] mapData->tiles`
- Unreal TArray automatically manages its own memory
