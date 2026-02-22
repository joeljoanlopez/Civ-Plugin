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

    int GetQ() const;
    int GetR() const;
    int GetS() const;

    int GetDistance(HexCoord other) const;

    bool operator==(const HexCoord& other) const;
    bool operator<(const HexCoord &other) const;
};