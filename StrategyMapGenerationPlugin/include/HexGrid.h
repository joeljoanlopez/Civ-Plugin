#pragma once
#include <vector>

#include "CoreAPI.h"
#include "HexCoord.h"

class MAPGEN_API HexGrid {
public:
    HexGrid(int w, int h);

    int GetWidth() const;
    int GetHeight() const;

    int GetTotalCells() const;

    HexCoord GetCenter() const;


    std::vector<HexCoord> GetNeighbors(HexCoord coord) const;

    bool IsInBounds(HexCoord coord) const;

private:
    static HexCoord OffsetToAxis(int x, int y);
    static std::pair<int, int> AxisToOffset(HexCoord coord);

    int width;
    int height;
    std::vector<HexCoord> coordinates;
};
