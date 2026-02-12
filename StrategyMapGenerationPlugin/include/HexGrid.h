#pragma once
#include <list>
#include <vector>

#include "CoreAPI.h"
#include "HexCoord.h"

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
    HexCoord GetHexCoord(int index) const;
    void AddTectonicCenters(std::list<HexCoord> centers);
    void FillTectonicPlates();
    int GetTectonicPlateAt(HexCoord coord) const;
    int AxisToIndex(HexCoord coord) const;

    HexCoord& GetMutableHexCoord(int index);
private:
    static HexCoord OffsetToAxis(int x, int y);
    static std::pair<int, int> AxisToOffset(HexCoord coord);

    int width;
    int height;
    std::vector<HexCoord> coordinates;
    std::list<int> tectonicCenters;
};
