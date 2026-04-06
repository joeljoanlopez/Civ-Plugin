# Map Generation Plugin for Unreal Engine

This plugin provides procedural hex-based map generation for Unreal Engine, wrapping the StrategyMapGenerationPlugin C++ library.

## Overview

The plugin dynamically includes the C++ source code from `StrategyMapGenerationPlugin` without copying files or using compiled `.so` libraries. All source files are referenced via relative paths.

## Architecture

### Source Structure

```
Civ_Unreal_Wrapper/Plugins/MapGenPlugin/
├── Source/MapGenPlugin/
│   ├── MapGenPlugin.Build.cs          # Build configuration with relative paths
│   ├── Public/
│   │   ├── MapGenPlugin.h             # Module header
│   │   └── MapGeneratorWrapper.h      # Main wrapper class
│   └── Private/
│       ├── MapGenPlugin.cpp           # Module implementation
│       ├── MapGeneratorWrapper.cpp    # Wrapper implementation
│       ├── MapGenCore_API.cpp         # Includes API source
│       ├── MapGenCore_Generation.cpp  # Includes generation sources
│       └── MapGenCore_Hex.cpp         # Includes hex grid sources
└── MapGenPlugin.uplugin               # Plugin descriptor
```

### Dynamic Source Inclusion

The plugin includes C++ source files from `StrategyMapGenerationPlugin` using relative paths:

- **MapGenCore_API.cpp** - Includes `MapGenerationAPI.cpp`
- **MapGenCore_Generation.cpp** - Includes Perlin noise, random, and tectonics generators
- **MapGenCore_Hex.cpp** - Includes hex coordinate, grid, and tile implementations

### Build Configuration

`MapGenPlugin.Build.cs` configures:
- **PublicIncludePaths**: Points to `StrategyMapGenerationPlugin/include`
- **PrivateIncludePaths**: Points to `StrategyMapGenerationPlugin/src`
- **PublicDefinitions**: Adds `MAPGEN_EXPORTS=1` for API exports
- **bUseUnity**: Disabled to prevent PCH conflicts with third-party code

## Usage

### Blueprint Usage

1. Add `AMapGeneratorWrapper` actor to your level
2. Configure generation parameters in the Details panel:
   - **Grid Settings**: Width, Height, Seed
   - **Random Config**: PlateCount, LandRatio, NoiseOctaves
   - **Visualization**: Toggle display options
3. Map generates automatically on BeginPlay
4. Call `RegenerateMap()` to generate a new map

### C++ Usage

```cpp
#include "MapGeneratorWrapper.h"

// Create and configure wrapper
AMapGeneratorWrapper* MapGen = GetWorld()->SpawnActor<AMapGeneratorWrapper>();
MapGen->Width = 10;
MapGen->Height = 8;
MapGen->Seed = 42;
MapGen->PlateCount = 8;
MapGen->LandRatio = 0.6f;
MapGen->NoiseOctaves = 4;

// Generate map
if (MapGen->GenerateMap())
{
    // Access generated tiles
    for (const FMapGenTileData& Tile : MapGen->Tiles)
    {
        UE_LOG(LogTemp, Log, TEXT("Tile (%d,%d): %s, Height: %.2f"),
            Tile.Q, Tile.R,
            *UEnum::GetDisplayValueAsText(Tile.Terrain).ToString(),
            Tile.Height);
    }
}
```

### Data Structures

#### FMapGenTileData
- **Q, R**: Hex coordinate (axial coordinates)
- **TectonicPlateId**: ID of the tectonic plate (0-indexed)
- **bIsLand**: Whether the tile is land or water
- **Height**: Elevation value (0.0 - 1.0)
- **Terrain**: Terrain type (DeepOcean, Water, Coast, Land, Mountain)

#### ETerrainType
- **DeepOcean** (0): Deep water
- **Water** (1): Shallow water
- **Coast** (2): Coastal area
- **Land** (3): Standard land
- **Mountain** (4): Elevated terrain

## Visualization

The wrapper provides debug visualization:
- Hexagon outlines in the viewport
- Color-coded terrain types
- Optional labels showing terrain, plate ID, height, and coordinates

Enable/disable visualization options in the actor's details panel.

## Comparison with Unity Implementation

### Unity (C#)
- Uses P/Invoke with DllImport to call compiled `.so`/`.dll`
- Marshals data between C# and C
- Requires compiled library

### Unreal (C++)
- Directly includes C++ source files
- No marshaling needed (native C++)
- No compiled library required
- Same API and data structures

## Notes

- The plugin automatically handles memory management via `MapGenFreeMap()`
- Unreal macros (`check`, `verify`) are temporarily disabled to avoid conflicts with standard C++
- Map regenerates automatically in editor when parameters change
- Debug visualization persists in editor mode
