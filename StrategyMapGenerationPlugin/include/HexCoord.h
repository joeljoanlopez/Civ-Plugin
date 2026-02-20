#pragma once
#include <cmath>
#include "CoreAPI.h"

enum class TerrainType {
    DeepOcean,
    Water,
    Coast,
    Land,
    Mountain
};

class MAPGEN_API HexCoord {
private:
    int q;
    int r;
    int tectonicPlateId;
    bool isLand;

    float height;
    TerrainType terrain;

public:
    HexCoord();
    HexCoord(int q, int r);

    int GetQ() const;
    int GetR() const;
    int GetS() const;

    int GetTectonicPlateId() const;
    void SetTectonicPlateId(int id);

    void SetLand(bool land);

    bool IsLand() const;

    int GetDistance(HexCoord other);

    bool operator==(const HexCoord& other) const;
    bool operator<(const HexCoord &other) const;

    float GetHeight() const;
    void SetHeight(float h);

    TerrainType GetTerrain() const;
    void SetTerrain(TerrainType t);

};