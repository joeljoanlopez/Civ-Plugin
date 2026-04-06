# Implementation Comparison: Unity vs Unreal Map Generator Wrapper

## Overview

Both wrappers provide the same functionality - wrapping the StrategyMapGenerationPlugin C++ library - but use different approaches based on their platform requirements.

## Key Differences

| Aspect | Unity (C#) | Unreal (C++) |
|--------|-----------|--------------|
| **Language** | C# | C++ |
| **Integration Method** | P/Invoke (DllImport) | Direct source inclusion |
| **Library Form** | Compiled `.so`/`.dll` | Source files via relative paths |
| **Data Marshaling** | Required (C# ↔ C) | Not required (native C++) |
| **Memory Management** | Manual via IntPtr | Direct pointer management |
| **Wrapper Type** | MonoBehaviour | AActor |

## File Structure Comparison

### Unity
```
Unity_Project/Assets/Plugins/
├── MapGeneratorWrapper.cs      # C# wrapper class
├── libMapGenCore.so            # Compiled Linux library
└── Editor/                     # Editor extensions
```

### Unreal
```
Civ_Unreal_Wrapper/Plugins/MapGenPlugin/
├── Source/MapGenPlugin/
│   ├── MapGenPlugin.Build.cs           # Build config with relative paths
│   ├── Public/
│   │   └── MapGeneratorWrapper.h       # Header with Unreal types
│   └── Private/
│       ├── MapGeneratorWrapper.cpp     # Implementation
│       ├── MapGenCore_API.cpp          # Includes API source
│       ├── MapGenCore_Generation.cpp   # Includes generation sources
│       └── MapGenCore_Hex.cpp          # Includes hex sources
└── MapGenPlugin.uplugin
```

## API Comparison

### Unity (C#)

```csharp
// Import C functions
[DllImport("libMapGenCore", CallingConvention = CallingConvention.Cdecl)]
private static extern int MapGenGenerateMap(
    int width, int height, int seed, int plateCount,
    float landRatio, int noiseOctaves, ref MapGenMapData outMap);

// Marshal data
tiles = new MapGenTileData[currentMap.tileCount];
int structSize = Marshal.SizeOf(typeof(MapGenTileData));
for (int i = 0; i < currentMap.tileCount; i++)
{
    IntPtr tilePtr = new IntPtr(currentMap.tiles.ToInt64() + (i * structSize));
    tiles[i] = Marshal.PtrToStructure<MapGenTileData>(tilePtr);
}
```

### Unreal (C++)

```cpp
// Direct C++ function call (no import needed)
int32 Result = MapGenGenerateMap(
    Width, Height, Seed, PlateCount, LandRatio, NoiseOctaves, &CurrentMapData);

// Direct memory access (no marshaling)
Tiles.SetNum(CurrentMapData.tileCount);
for (int32 i = 0; i < CurrentMapData.tileCount; ++i)
{
    const MapGenTileData& SrcTile = CurrentMapData.tiles[i];
    FMapGenTileData& DstTile = Tiles[i];
    DstTile.Q = SrcTile.q;
    // ... direct assignment
}
```

## Data Structure Mapping

### C API (StrategyMapGenerationPlugin)
```c
typedef struct MapGenTileData {
    int q;
    int r;
    int tectonicPlateId;
    int isLand;
    float height;
    int terrain;
} MapGenTileData;
```

### Unity (C#)
```csharp
public struct MapGenTileData
{
    public int q;
    public int r;
    public int tectonicPlateId;
    public int isLand;
    public float height;
    public int terrain;
}
```

### Unreal (C++)
```cpp
USTRUCT(BlueprintType)
struct FMapGenTileData
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly, Category = "Map Generation")
    int32 Q;
    
    UPROPERTY(BlueprintReadOnly, Category = "Map Generation")
    int32 R;
    
    // ... Unreal-style properties with UPROPERTY macros
};
```

## Visualization Comparison

### Unity
- Uses Gizmos and Handles API
- Editor-only visualization
- OnDrawGizmos() for persistent display

```csharp
void OnDrawGizmos()
{
    Gizmos.color = GetTerrainColor((TerrainType)tile.terrain);
    DrawHexagon(center, hexSize);
    Handles.Label(center, label);
}
```

### Unreal
- Uses DrawDebugHelpers
- Runtime and editor visualization
- Persistent debug lines

```cpp
void DrawDebugHexGrid()
{
    DrawDebugLine(GetWorld(), Vertices[i], Vertices[Next], EdgeColor, true, -1.0f);
    DrawDebugString(GetWorld(), Center, Label, nullptr, FColor::White, -1.0f);
}
```

## Build System Integration

### Unity
- Simple file drop into Assets/Plugins/
- .so file must be pre-compiled for target platform
- Automatic platform detection via preprocessor directives

```csharp
#if UNITY_EDITOR_WIN || UNITY_STANDALONE_WIN
    private const string DllName = "MapGenCore";
#else
    private const string DllName = "libMapGenCore";
#endif
```

### Unreal
- Unreal Build Tool (UBT) configuration
- Source files included via relative paths
- No pre-compilation needed

```csharp
// MapGenPlugin.Build.cs
string MapGenCorePath = Path.GetFullPath(
    Path.Combine(ModuleDirectory, "../../../../../StrategyMapGenerationPlugin"));
PublicIncludePaths.AddRange(new string[] { MapGenCoreIncludePath });
```

## Advantages

### Unity Approach
✅ Clean separation between C# and C++  
✅ Standard DLL/SO distribution model  
✅ Platform-specific binaries can be optimized  
✅ No build-time dependencies on source  

❌ Requires pre-compiled libraries  
❌ Data marshaling overhead  
❌ Platform-specific builds needed  

### Unreal Approach
✅ No compiled libraries needed  
✅ No marshaling overhead  
✅ Direct C++ integration  
✅ Single source of truth  
✅ Easier debugging (direct source access)  

❌ Tighter coupling to source code  
❌ Relative paths can be fragile  
❌ Longer compile times  

## Usage Examples

Both wrappers provide similar high-level interfaces:

### Unity
```csharp
MapGeneratorWrapper mapGen = gameObject.AddComponent<MapGeneratorWrapper>();
mapGen.width = 10;
mapGen.height = 8;
mapGen.seed = 42;
mapGen.GenerateMap();
```

### Unreal (Blueprint)
- Add AMapGeneratorWrapper actor to level
- Set properties in Details panel
- Automatic generation on BeginPlay

### Unreal (C++)
```cpp
AMapGeneratorWrapper* MapGen = GetWorld()->SpawnActor<AMapGeneratorWrapper>();
MapGen->Width = 10;
MapGen->Height = 8;
MapGen->Seed = 42;
MapGen->GenerateMap();
```

## Conclusion

The Unity wrapper uses standard FFI (Foreign Function Interface) patterns with compiled libraries, while the Unreal wrapper leverages direct C++ source inclusion. Both approaches are valid and appropriate for their respective engines, with the Unreal approach being more straightforward due to both the engine and library being C++-based.
