# Using MapGeneratorWrapper in Blueprint - Step-by-Step Guide

## Part 1: Basic Setup and Visualization

### 1. Add the Actor to Your Level

**Option A: Use Your Blueprint**
1. In Content Browser, find your Blueprint (the one derived from AMapGeneratorWrapper)
2. Drag it into the level viewport
3. Position it wherever you want (the map will generate at this location)

**Option B: Use the Base Class Directly**
1. In the Place Actors panel, search for "MapGeneratorWrapper"
2. Drag `AMapGeneratorWrapper` into your level

### 2. Configure Generation Parameters

Select the actor in the viewport, then in the Details panel you'll see:

**Grid Settings:**
- **Width**: Number of hex columns (default: 8, range: 4-50)
- **Height**: Number of hex rows (default: 6, range: 4-50)  
- **Seed**: Random seed for reproducible maps (default: 1234)

**Random Config:**
- **Plate Count**: Number of tectonic plates (default: 6, range: 2-15)
- **Land Ratio**: Percentage of land vs water (default: 0.5, range: 0.0-1.0)
- **Noise Octaves**: Terrain detail level (default: 3, range: 1-5)

**Visualization:**
- **Show Terrain**: Display terrain type names
- **Show Plate Id**: Display tectonic plate IDs
- **Show Height**: Display elevation values
- **Show Coordinates**: Display hex coordinates (Q, R)

### 3. See the Map

1. Click **Play** (or Alt+P)
2. The map automatically generates when the game starts (BeginPlay event)
3. You should see:
   - Colored hexagons representing different terrain types
   - Optional labels (if visualization options are enabled)

**Terrain Colors:**
- **Dark Blue**: Deep Ocean
- **Medium Blue**: Water
- **Sandy/Beige**: Coast
- **Green**: Land
- **Gray**: Mountain

### 4. Regenerate the Map

To generate a new map while playing:
1. Open your Blueprint in the editor
2. In the Event Graph, call `RegenerateMap` (see Part 2)

OR

Change any parameter in the Details panel (in editor mode) and the map auto-regenerates!

---

## Part 2: Using Functions in Blueprint

### Open Your Blueprint Event Graph

1. Double-click your Blueprint in Content Browser
2. Go to the **Event Graph** tab

### A. Generate Map on Begin Play (Already Done)

The map automatically generates when the game starts. This is built-in, but here's how it works:

```
Event BeginPlay → (internal GenerateMap call)
```

### B. Call GenerateMap Manually

To regenerate the map with specific settings:

1. **Right-click** in Event Graph → Search "GenerateMap"
2. Connect it to an event (like a key press)

Example - Press 'G' to generate:
```
Event Graph:
┌─────────────────┐     ┌──────────────┐
│ InputAction G   │────▶│ GenerateMap  │
└─────────────────┘     └──────────────┘
                              │
                              ▼
                        ┌──────────────┐
                        │ Branch       │
                        └──────────────┘
                         │True   │False
                         ▼       ▼
                   (Success)  (Failed)
```

**Steps:**
1. Add Event: `Event Tick` or `InputAction` (keyboard)
2. Add Function Call: `GenerateMap`
3. `GenerateMap` returns a **boolean** (true = success, false = failed)
4. Use a **Branch** node to check the result

### C. Call RegenerateMap

Even simpler - doesn't return a value:

```
Event Graph:
┌─────────────────┐     ┌──────────────────┐
│ InputAction R   │────▶│ RegenerateMap    │
└─────────────────┘     └──────────────────┘
```

### D. Change Parameters Before Generating

```
Event Graph:
┌─────────────────┐     ┌──────────────┐     ┌──────────────┐
│ Event Begin Play│────▶│ Set Seed     │────▶│ GenerateMap  │
└─────────────────┘     │ (New Value)  │     └──────────────┘
                        └──────────────┘
```

**Steps:**
1. Drag your actor reference into the graph (or use `Get a reference to self`)
2. Drag from the reference pin → Search "Set Width" (or any parameter)
3. Connect the setter → Then call `GenerateMap` or `RegenerateMap`

### E. Access Generated Tile Data

The `Tiles` array contains all generated tiles. Each tile has:
- **Q**: Hex coordinate (column)
- **R**: Hex coordinate (row)
- **Tectonic Plate Id**: Which plate (0-indexed)
- **Is Land**: Boolean (true = land, false = water)
- **Height**: Float (0.0 - 1.0)
- **Terrain**: Enum (Deep Ocean, Water, Coast, Land, Mountain)

**Example: Count Land Tiles**

```
Event Graph:
┌──────────────┐     ┌──────────────┐     ┌──────────────┐
│ GenerateMap  │────▶│ Get Tiles    │────▶│ ForEachLoop  │
└──────────────┘     └──────────────┘     └──────────────┘
                                                 │
                                                 ▼ (Loop Body)
                                           ┌──────────────┐
                                           │ Branch       │
                                           │ (Is Land?)   │
                                           └──────────────┘
                                                 │ True
                                                 ▼
                                           ┌──────────────┐
                                           │ Increment    │
                                           │ Counter      │
                                           └──────────────┘
```

**Steps:**
1. Call `GenerateMap`
2. Get the `Tiles` variable
3. Add `ForEachLoop` node
4. In loop body, access `Array Element` → `Is Land`
5. Use a **Branch** to check if it's land
6. Count or process as needed

### F. Spawn Actors on Tiles

Example: Spawn a tree on each land tile

```
Event Graph:
┌──────────────┐     ┌──────────────┐     ┌──────────────┐
│ GenerateMap  │────▶│ Get Tiles    │────▶│ ForEachLoop  │
└──────────────┘     └──────────────┘     └──────────────┘
                                                 │
                                                 ▼ (Loop Body)
                                           ┌──────────────┐
                                           │ Branch       │
                                           │ (Is Land?)   │
                                           └──────────────┘
                                                 │ True
                                                 ▼
                                           ┌──────────────┐
                                           │ Calculate    │
                                           │ World Pos    │
                                           └──────────────┘
                                                 │
                                                 ▼
                                           ┌──────────────┐
                                           │ Spawn Actor  │
                                           │ (Tree)       │
                                           └──────────────┘
```

**Calculate World Position from Hex Coordinates:**
```
X = HexSize * sqrt(3) * (Q + R / 2)
Y = -HexSize * (3 / 2) * R
Z = 0 (or terrain height)

HexSize = 100.0 (default, in Unreal units)
```

**Blueprint nodes needed:**
1. Get `Q` and `R` from tile
2. Math operations to calculate X, Y
3. `Make Vector` with calculated X, Y, Z
4. `Spawn Actor from Class` at that location

---

## Part 3: Complete Blueprint Examples

### Example 1: Press 'R' to Randomize Map

**Event Graph:**
1. Add Event: **Input Action R** (or **Keyboard R**)
2. Connect to: **Set Seed** (use `Random Integer in Range` node, 0-99999)
3. Connect to: **Regenerate Map**

### Example 2: Print Map Statistics

**Event Graph:**
```
After GenerateMap succeeds:
1. Get Tiles → Get Length → Print String ("Total tiles: X")
2. Get Tiles → ForEachLoop → Branch (Is Land?) → Increment counter
3. Print String ("Land tiles: X")
```

### Example 3: Get Terrain Color for UI

Use the **Get Terrain Color** function:

```
┌──────────────┐     ┌──────────────────┐     ┌──────────────┐
│ Get Tile[i]  │────▶│ Get Terrain      │────▶│ GetTerrainColor│
└──────────────┘     └──────────────────┘     │ (Function)     │
                                               └──────────────┘
                                                     │
                                                     ▼
                                               Linear Color
                                               (use for UI)
```

---

## Part 4: Common Blueprint Patterns

### Pattern: Store Reference to MapGen Actor

In your Level Blueprint or Game Mode:

1. Add Variable: `MapGenerator` (type: `Map Generator Wrapper` - Object Reference)
2. On BeginPlay: `Get Actor Of Class` → `Map Generator Wrapper`
3. Store in variable
4. Use the reference anywhere to call functions

### Pattern: React When Map Generates

Currently, the map generates synchronously, so you can just:
```
GenerateMap → (next node executes after generation completes)
```

### Pattern: Different Map Presets

Create a `struct` with preset parameters:

**Small Map Preset:**
- Width: 6, Height: 4
- PlateCount: 3
- LandRatio: 0.5

**Large Map Preset:**
- Width: 40, Height: 30
- PlateCount: 12
- LandRatio: 0.55

Apply via **Set Members in Struct** before calling GenerateMap.

---

## Part 5: Debugging Tips

### View Tile Data

1. While playing, select the MapGeneratorWrapper actor
2. In Details panel, expand **Tiles** array
3. Inspect individual tile properties

### Enable All Visualization Options

In actor Details:
- ✅ Show Terrain
- ✅ Show Plate Id  
- ✅ Show Height
- ✅ Show Coordinates

This helps understand the generated map.

### Print Tile Info

In Blueprint:
```
Get Tiles[0] → Break MapGenTileData → Print String
```

This shows the structure of a tile.

---

## Quick Reference

**Key Functions:**
- `GenerateMap()` → bool - Generates map, returns success
- `RegenerateMap()` - Regenerates with current parameters
- `GetTerrainColor(Terrain)` → LinearColor - Gets color for terrain type

**Key Variables:**
- `Tiles` - Array of all generated tiles (read-only)
- `Width`, `Height`, `Seed` - Generation parameters (read/write)
- `PlateCount`, `LandRatio`, `NoiseOctaves` - Config (read/write)

**Tile Properties:**
- `Q`, `R` - Hex coordinates
- `TectonicPlateId` - Plate number
- `Is Land` - Boolean
- `Height` - Float (0-1)
- `Terrain` - Enum value

---

## Need Help?

See also:
- `QUICK_START.md` - C++ examples
- `README.md` - Technical details
- `IMPLEMENTATION_COMPARISON.md` - Unity vs Unreal
