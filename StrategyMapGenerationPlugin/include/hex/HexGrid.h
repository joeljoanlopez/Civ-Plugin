#pragma once
#include <map>
#include <vector>

#include "core/CoreAPI.h"
#include "HexCoord.h"
#include "HexTile.h"

class MAPGEN_API HexGrid {
public:
    HexGrid(int w, int h);

    int GetWidth() const;
    int GetHeight() const;
    int GetTotalCells() const;

    [[nodiscard]] HexCoord GetGridCenter() const;

    [[nodiscard]] int OffsetToIndex(int x, int y) const;
    [[nodiscard]] std::vector<HexCoord> GetNeighbors(HexCoord coord) const;
    [[nodiscard]] bool IsInBounds(HexCoord coord) const;
    [[nodiscard]] HexCoord GetCoordAt(int index) const;
    [[nodiscard]] int CoordToIndex(HexCoord coord) const;
    
    HexTile& GetTileAt(HexCoord coord);
    [[nodiscard]] const HexTile& GetTileAt(HexCoord coord) const;
    HexTile& GetTileAt(int index);
    [[nodiscard]] const HexTile& GetTileAt(int index) const;
    [[nodiscard]] inline auto begin() const { return tiles.begin(); }
    [[nodiscard]] inline auto end() const { return tiles.end(); }
private:
    static HexCoord OffsetToAxis(int x, int y);
    static std::pair<int, int> AxisToOffset(HexCoord coord);

    int width;
    int height;
    std::map<HexCoord, HexTile> tiles;
    std::vector<HexCoord> coordinates;
};


//COMPILATION UNIT (CPP QUE INCLOU CPPS)