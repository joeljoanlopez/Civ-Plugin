# Quick Start Guide - MapGenPlugin for Unreal

## What This Plugin Does

Generates procedural hex-based strategy maps using tectonic plate simulation and Perlin noise. Similar to Civilization-style world generation.

## Installation

The plugin is already set up in `Civ_Unreal_Wrapper/Plugins/MapGenPlugin/`.

No additional installation needed - it dynamically references the source code from `StrategyMapGenerationPlugin/`.

## Basic Usage (Blueprint)

1. **Add Actor to Level**
   - Drag `AMapGeneratorWrapper` into your level

2. **Configure Parameters** (in Details panel)
   - **Width**: 4-50 (number of hex columns)
   - **Height**: 4-50 (number of hex rows)
   - **Seed**: Any integer (for reproducible maps)
   - **Plate Count**: 2-15 (tectonic plates)
   - **Land Ratio**: 0.0-1.0 (percentage of land vs water)
   - **Noise Octaves**: 1-5 (terrain detail level)

3. **Visualize**
   - Map generates automatically on play
   - Toggle visualization options in Details panel
   - See hexagon grid with color-coded terrain

4. **Regenerate**
   - Call `RegenerateMap()` function
   - Or change any parameter in editor (auto-regenerates)

## Basic Usage (C++)

```cpp
#include "MapGeneratorWrapper.h"

void AMyGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    // Spawn map generator
    AMapGeneratorWrapper* MapGen = GetWorld()->SpawnActor<AMapGeneratorWrapper>();
    
    // Configure
    MapGen->Width = 12;
    MapGen->Height = 10;
    MapGen->Seed = 42;
    MapGen->PlateCount = 8;
    MapGen->LandRatio = 0.6f;
    
    // Generate
    if (MapGen->GenerateMap())
    {
        // Access tiles
        for (const FMapGenTileData& Tile : MapGen->Tiles)
        {
            if (Tile.bIsLand)
            {
                // Do something with land tile
                FString TerrainName = UEnum::GetDisplayValueAsText(Tile.Terrain).ToString();
                UE_LOG(LogTemp, Log, TEXT("Land tile at (%d,%d): %s"), 
                    Tile.Q, Tile.R, *TerrainName);
            }
        }
    }
}
```

## Understanding the Data

### Tile Data (FMapGenTileData)

```cpp
struct FMapGenTileData
{
    int32 Q;                    // Hex coordinate Q (column)
    int32 R;                    // Hex coordinate R (row)
    int32 TectonicPlateId;      // Which plate this tile belongs to
    bool bIsLand;               // Is this land or water?
    float Height;               // Elevation (0.0 - 1.0)
    ETerrainType Terrain;       // Terrain classification
};
```

### Terrain Types

- **DeepOcean**: Deep water (darkest blue)
- **Water**: Shallow water (medium blue)
- **Coast**: Coastal area (sandy color)
- **Land**: Standard terrain (green)
- **Mountain**: High elevation (gray)

### Hex Coordinates

Uses axial coordinates (Q, R):
- **Q**: Column (increases to the right)
- **R**: Row (increases downward-right)
- **S** = -Q - R (derived, not stored)

Convert to world position:
```cpp
float X = HexSize * sqrt(3.0f) * (Q + R / 2.0f);
float Y = -HexSize * (3.0f / 2.0f) * R;
```

## Common Workflows

### Generate Multiple Maps
```cpp
for (int i = 0; i < 10; ++i)
{
    MapGen->Seed = i;
    MapGen->GenerateMap();
    // Use MapGen->Tiles data
}
```

### Find All Land Tiles
```cpp
TArray<FMapGenTileData> LandTiles;
for (const FMapGenTileData& Tile : MapGen->Tiles)
{
    if (Tile.bIsLand)
    {
        LandTiles.Add(Tile);
    }
}
```

### Get Tiles by Terrain Type
```cpp
TArray<FMapGenTileData> Mountains;
for (const FMapGenTileData& Tile : MapGen->Tiles)
{
    if (Tile.Terrain == ETerrainType::Mountain)
    {
        Mountains.Add(Tile);
    }
}
```

### Spawn Actors on Map
```cpp
for (const FMapGenTileData& Tile : MapGen->Tiles)
{
    float X = 100.0f * FMath::Sqrt(3.0f) * (Tile.Q + Tile.R / 2.0f);
    float Y = -100.0f * (3.0f / 2.0f) * Tile.R;
    FVector Location = MapGen->GetActorLocation() + FVector(X, Y, 0);
    
    // Spawn different actors based on terrain
    switch (Tile.Terrain)
    {
        case ETerrainType::Mountain:
            GetWorld()->SpawnActor<AMountainActor>(Location, FRotator::ZeroRotator);
            break;
        case ETerrainType::Land:
            GetWorld()->SpawnActor<AGrassActor>(Location, FRotator::ZeroRotator);
            break;
        // etc.
    }
}
```

## Troubleshooting

### Plugin won't compile
- Verify `StrategyMapGenerationPlugin` exists in repository root
- Check relative paths in `MapGenPlugin.Build.cs`
- Ensure all source files exist in `StrategyMapGenerationPlugin/src/`

### No visualization
- Make sure actor is in the viewport
- Check that visualization booleans are enabled
- Verify `GenerateMap()` returned true

### Map generation fails
- Check parameters are within valid ranges
- Width and Height must be > 0
- PlateCount must be > 0
- LandRatio must be 0.0 - 1.0
- Check log for error messages

## Next Steps

- See `README.md` for architecture details
- See `IMPLEMENTATION_COMPARISON.md` for Unity vs Unreal comparison
- See `SOURCE_MAPPING.md` for file structure details

## Parameter Recommendations

### Small Test Map
- Width: 6, Height: 4
- PlateCount: 3
- LandRatio: 0.5

### Medium Game Map
- Width: 20, Height: 15
- PlateCount: 8
- LandRatio: 0.6

### Large World Map
- Width: 40, Height: 30
- PlateCount: 12
- LandRatio: 0.55
