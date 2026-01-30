#pragma once
#include "CoreAPI.h"
#include "HexCoord.h"

class MAPGEN_API HexGrid {
public:
    HexGrid(int w, int h);

    int GetWidth() const;
    int GetHeight() const;
    int GetTotalCells() const;

    static HexCoord OffsetToAxis(int x, int y);

private:
    int width;
    int height;
};
