#include "HexGrid.h"

#include "HexCoord.h"

HexGrid::HexGrid(int w, int h) {
    this->width = w;
    this->height = h;
}

int HexGrid::GetWidth() const {
    return this->width;
}

int HexGrid::GetHeight() const {
    return this->height;
}

int HexGrid::GetTotalCells() const {
    return this->width * this->height;
}

HexCoord HexGrid::OffsetToAxis(int x, int y) {
    int parity = y % 2 == 0 ? 0 : 1;
    int q = x - (y - parity) / 2;
    int r = y;
    return HexCoord(q, r);
}
