#pragma once
#include "CoreAPI.h"

enum class TerrainType {
    DeepOcean,
    Water,
    Coast,
    Land,
    Mountain
};

class MAPGEN_API HexTile {
private:
    int tectonicPlateId;
    bool isLand;
    float height;
    TerrainType terrain;

public:
    HexTile();

    int GetTectonicPlateId() const;
    void SetTectonicPlateId(int id);

    void SetLand(bool land);
    bool IsLand() const;

    float GetHeight() const;
    void SetHeight(float h);

    TerrainType GetTerrain() const;
    void SetTerrain(TerrainType t);
};
