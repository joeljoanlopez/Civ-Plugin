#pragma once
#include <cmath>
#include <algorithm>
#include "core/CoreAPI.h"

class MAPGEN_API HexCoord {
private:
    int q;
    int r;

public:
    HexCoord();
    HexCoord(int q, int r);

    [[nodiscard]] inline int GetQ() const { return q; }
    [[nodiscard]] inline int GetR() const { return r; }
    [[nodiscard]] inline int GetS() const { return -q - r; }

    [[nodiscard]] int GetDistance(HexCoord other) const;

    [[nodiscard]] bool operator==(const HexCoord& other) const;
    [[nodiscard]] bool operator<(const HexCoord &other) const;
};