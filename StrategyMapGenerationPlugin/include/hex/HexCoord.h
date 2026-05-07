#pragma once
#include <algorithm>
#include <functional>
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

template<>
struct std::hash<HexCoord> {
    size_t operator()(const HexCoord& c) const noexcept {
        const size_t hq = std::hash<int>{}(c.GetQ());
        const size_t hr = std::hash<int>{}(c.GetR());
        return hq ^ (hr * 2654435761u);
    }
};