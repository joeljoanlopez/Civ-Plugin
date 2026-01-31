#include "HexGrid.h"

#include <iostream>

#include "HexCoord.h"

HexGrid::HexGrid(int w, int h) {
    this->width = w;
    this->height = h;

    this->coordinates.assign(w*h, HexCoord());
    for (int r = 0; r < h; r++) {
        for (int q = 0; q < w; q++) {
            int index = r * w + q;
            this->coordinates[index] = OffsetToAxis(q, r);
        }
    }
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

HexCoord HexGrid::GetCenter() const {
    int centerX = this->width / 2;
    int centerY = this->height / 2;
    return {centerX, centerY};
}

HexCoord HexGrid::GetCoord(HexCoord pos) const {
    int index = pos.GetR() * this->width + pos.GetQ();
    if (index >= 0 && index < (int)this->coordinates.size()) {
        return this->coordinates[index];
    }
    return {-1, -1};
}

HexCoord HexGrid::OffsetToAxis(int x, int y) {
    int parity = y % 2 == 0 ? 0 : 1;
    int q = x - (y - parity) / 2;
    int r = y;
    return {q, r};
}

std::vector<HexCoord> HexGrid::GetNeighbors(HexCoord coord) const {
    std::vector<HexCoord> neighbors;
    std::vector<HexCoord> directions = {
        {1, 0}, {0, 1}, {-1, 1},
        {-1, 0}, {0, -1}, {1, -1}
    };

    for (int i = 0; i < directions.size(); i++) {
        HexCoord neighborCoord(coord.GetQ() + directions[i].GetQ(), coord.GetR() + directions[i].GetR());

        // Check if neighbor is within grid bounds
        if (neighborCoord.GetQ() >= 0 && neighborCoord.GetQ() < this->width &&
            neighborCoord.GetR() >= 0 && neighborCoord.GetR() < this->height) {
            neighbors.push_back(neighborCoord);
        }
    }

    return neighbors;
}