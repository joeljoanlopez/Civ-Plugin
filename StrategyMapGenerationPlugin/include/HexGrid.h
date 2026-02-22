#pragma once
#include <map>
#include <vector>

#include "CoreAPI.h"
#include "HexCoord.h"
#include "HexTile.h"

class MAPGEN_API HexGrid {
public:
    HexGrid(int w, int h);

    int GetWidth() const;
    int GetHeight() const;
    int GetTotalCells() const;

    HexCoord GetGridCenter() const;

    int OffsetToIndex(int x, int y) const;
    std::vector<HexCoord> GetNeighbors(HexCoord coord) const;
    bool IsInBounds(HexCoord coord) const;
    HexCoord GetCoordAt(int index) const;
    int CoordToIndex(HexCoord coord) const;
    
    HexTile& GetTileAt(HexCoord coord);
    const HexTile& GetTileAt(HexCoord coord) const;
    HexTile& GetTileAt(int index);
    const HexTile& GetTileAt(int index) const;

private:
    static HexCoord OffsetToAxis(int x, int y);
    static std::pair<int, int> AxisToOffset(HexCoord coord);

    int width;
    int height;
    std::map<HexCoord, HexTile> tiles;
    std::vector<HexCoord> coordinates;
};
