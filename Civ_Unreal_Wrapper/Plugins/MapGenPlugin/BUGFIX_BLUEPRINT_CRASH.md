# Bug Fix: Unreal Editor Crash When Creating Blueprint

## Problem

Unreal Editor crashed when attempting to create a Blueprint derived from `AMapGeneratorWrapper`. The crash appeared to manifest as Vulkan/Wayland display errors, but these were just symptoms of a deeper compilation issue.

## Root Cause

The original implementation included the C header file (`api/MapGenerationAPI.h`) directly in the public header file `MapGeneratorWrapper.h`, before the `.generated.h` include:

```cpp
// INCORRECT - causes Unreal Header Tool issues
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "api/MapGenerationAPI.h"        // ← C header with extern "C" functions
#include "MapGeneratorWrapper.generated.h"
```

This caused several issues:
1. **Unreal Header Tool (UHT) conflict**: The C API header contains `extern "C"` declarations that shouldn't be visible to Unreal's reflection system
2. **Incomplete type error**: Using `MapGenMapData` as a complete type (struct member) instead of a pointer
3. **Macro conflicts**: The C header's macros could interfere with Unreal's macros

## Solution

### 1. Remove C Header from Public Header

Changed `MapGeneratorWrapper.h`:
```cpp
// CORRECT
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapGeneratorWrapper.generated.h"

// Forward declare C struct to avoid including C header in .h file
struct MapGenMapData;
```

### 2. Use Pointer Instead of Value

Changed the private member from a value to a pointer:
```cpp
// Before:
private:
    MapGenMapData CurrentMapData;  // ← Requires complete type definition

// After:
private:
    MapGenMapData* CurrentMapData;  // ← Works with forward declaration
```

### 3. Move C Header Include to Implementation

Added the include to `MapGeneratorWrapper.cpp`:
```cpp
#include "MapGeneratorWrapper.h"
#include "DrawDebugHelpers.h"
#include "api/MapGenerationAPI.h"  // ← Now included in .cpp only
```

### 4. Update Pointer Usage

Updated all code to use the pointer:
```cpp
// Constructor
AMapGeneratorWrapper::AMapGeneratorWrapper()
{
    CurrentMapData = new MapGenMapData();
    CurrentMapData->tiles = nullptr;
    // ...
}

// Destructor/Cleanup
void AMapGeneratorWrapper::EndPlay(...)
{
    FreeCurrentMap();
    delete CurrentMapData;
    CurrentMapData = nullptr;
}

// Usage
MapGenGenerateMap(..., CurrentMapData);  // Pass pointer directly
if (CurrentMapData && CurrentMapData->tiles != nullptr) { ... }
```

## Why This Matters

### Unreal Header Tool (UHT) Requirements

Unreal's reflection system (UHT) processes header files to generate code for:
- UPROPERTY reflection
- Blueprint integration  
- Serialization
- Garbage collection

When you include C headers (especially with `extern "C"`), UHT can get confused because:
1. C headers don't follow Unreal naming conventions
2. C functions can't be reflected to Blueprint
3. C macros can conflict with Unreal macros

### Best Practice: Forward Declarations for External APIs

When wrapping external C/C++ libraries in Unreal:

**✅ DO:**
- Forward declare external types in header files
- Use pointers or references to forward-declared types
- Include external headers only in .cpp files
- Keep all UCLASS/USTRUCT/UENUM types pure Unreal

**❌ DON'T:**
- Include external library headers in public Unreal headers
- Use external types as complete types (values) in UCLASS
- Mix C extern declarations with Unreal reflection macros

## Result

After this fix:
- ✅ Plugin compiles successfully
- ✅ No Unreal Header Tool errors
- ✅ Blueprint creation should work properly
- ✅ C API is properly encapsulated in implementation file

## Files Changed

1. **Source/MapGenPlugin/Public/MapGeneratorWrapper.h**
   - Removed `#include "api/MapGenerationAPI.h"`
   - Added forward declaration `struct MapGenMapData;`
   - Changed `MapGenMapData CurrentMapData;` to `MapGenMapData* CurrentMapData;`

2. **Source/MapGenPlugin/Private/MapGeneratorWrapper.cpp**
   - Added `#include "api/MapGenerationAPI.h"`
   - Updated constructor to allocate pointer
   - Updated destructor to deallocate pointer
   - Updated all usages from `.` to `->`

## Testing

To verify the fix:
1. Open Unreal Editor
2. Enable MapGenPlugin
3. Create a new Blueprint Class
4. Select AMapGeneratorWrapper as parent
5. Blueprint editor should open without crashing
6. Compile and test the blueprint

## Lessons Learned

When wrapping external C/C++ libraries for Unreal:
1. Keep public headers clean and Unreal-focused
2. Use forward declarations and pointers for external types
3. Encapsulate external APIs in .cpp files
4. Let Unreal Header Tool only see Unreal-style code
5. Test early by creating Blueprints, not just C++ compilation
