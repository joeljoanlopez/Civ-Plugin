#pragma once
#include "core/CoreAPI.h"

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

    [[nodiscard]] int GetTectonicPlateId() const;
    void SetTectonicPlateId(int id);

    void SetLand(bool land);
    [[nodiscard]] bool IsLand() const;

    [[nodiscard]] float GetHeight() const;
    void SetHeight(float h);

    [[nodiscard]] TerrainType GetTerrain() const;
    void SetTerrain(TerrainType t);
};
