#include "HexGrid.h"
#include "HexCoord.h"
#include <iostream>

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
    this->tectonicCenters = std::list<int>();
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

HexCoord HexGrid::GetGridCenter() const {
    int centerX = this->width / 2;
    int centerY = this->height / 2;
    return {centerX, centerY};
}

int HexGrid::OffsetToIndex(int x, int y) const {
    return y * this->width + x;
}

HexCoord HexGrid::OffsetToAxis(int x, int y) {
    int parity = y % 2 == 0 ? 0 : 1;
    int q = x - (y - parity) / 2;
    int r = y;
    return {q, r};
}

std::pair<int, int> HexGrid::AxisToOffset(HexCoord coord) {
    int parity = coord.GetR() % 2 == 0 ? 0 : 1;
    int x = coord.GetQ() + (coord.GetR() - parity) / 2;
    int y = coord.GetR();
    return {x, y};
}

int HexGrid::AxisToIndex(HexCoord coord) const {
    auto [x, y] = AxisToOffset(coord);
    return OffsetToIndex(x, y);
}

std::vector<HexCoord> HexGrid::GetNeighbors(HexCoord coord) const {
    std::vector<HexCoord> neighbors;
    std::vector<HexCoord> directions = {
        {1, 0}, {0, 1}, {-1, 1},
        {-1, 0}, {0, -1}, {1, -1}
    };

    for (int i = 0; i < directions.size(); i++) {
        HexCoord neighborCoord(coord.GetQ() + directions[i].GetQ(), coord.GetR() + directions[i].GetR());

        if (IsInBounds(neighborCoord)) {
            neighbors.push_back(neighborCoord);
        }
    }

    return neighbors;
}

bool HexGrid::IsInBounds(HexCoord coord) const {
    auto [x, y] = AxisToOffset(coord);
    return x >= 0 && x < this->width && y >= 0 && y < this->height;
}

HexCoord HexGrid::GetHexCoord(int index) const {
    HexCoord coord = this->coordinates[index];

    if (!IsInBounds(coord)) {
        throw std::out_of_range("Index out of bounds");
    }

    return coord;
}

void HexGrid::AddTectonicCenters(std::list<HexCoord> centers) {
    for (HexCoord center: centers) {
        if (!IsInBounds(center)) {
            throw std::out_of_range("Tectonic center out of bounds");
        }

        int centerIndex = AxisToIndex(center);
        this->coordinates[centerIndex].SetTectonicPlateId(centerIndex);
        this->coordinates[centerIndex].SetLand(center.IsLand());
        this->tectonicCenters.push_back(centerIndex);
    }
}

void HexGrid::FillTectonicPlates() {
    for (HexCoord& coordinate : coordinates) {
        int nearestPlateId = -1;
        for (int centerIndex : tectonicCenters) {
            int distance = coordinate.GetDistance(coordinates[centerIndex]);
            if (nearestPlateId == -1 || distance < coordinate.GetDistance(coordinates[nearestPlateId])) {
                nearestPlateId = centerIndex;
            }
        }
        coordinate.SetTectonicPlateId(nearestPlateId);
        coordinate.SetLand(GetHexCoord(nearestPlateId).IsLand());
    }
}

int HexGrid::GetTectonicPlateAt(HexCoord coord) const {
    if (!IsInBounds(coord)) {
        throw std::out_of_range("Coordinate out of bounds");
    }

    int index = AxisToIndex(coord);
    return this->coordinates[index].GetTectonicPlateId();
}

HexCoord& HexGrid::GetMutableHexCoord(int index) {
    if (index >= 0 && index < coordinates.size()) {
        return coordinates[index];
    }
    return coordinates[0];
}