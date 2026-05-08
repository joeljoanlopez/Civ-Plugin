#pragma once
#include "core/CoreAPI.h"

class MAPGEN_API HexTile {
private:
    int tectonicPlateId;
    bool isLand;
    float height;
    int terrain;
    float temperature;
    float moisture;

public:
    HexTile();

    [[nodiscard]] int GetTectonicPlateId() const;
    void SetTectonicPlateId(int id);

    void SetLand(bool land);
    [[nodiscard]] bool IsLand() const;

    [[nodiscard]] float GetHeight() const;
    void SetHeight(float h);

    [[nodiscard]] int GetTerrain() const;
    void SetTerrain(int t);

    [[nodiscard]] float GetTemperature() const;
    void SetTemperature(float t);

    [[nodiscard]] float GetMoisture() const;
    void SetMoisture(float m);
};
