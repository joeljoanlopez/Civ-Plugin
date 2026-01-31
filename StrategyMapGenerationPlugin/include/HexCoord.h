#pragma once
#include "CoreAPI.h"

struct MAPGEN_API HexCoord {
private:
    int q;
    int r;

public:
    HexCoord();
    HexCoord(int q, int r);

    int GetQ() const;
    int GetR() const;
    int GetS() const;

    bool operator==(const HexCoord& other) const;
};