#include "HexTile.h"

HexTile::HexTile() {
    this->tectonicPlateId = -1;
    this->isLand = false;
    this->height = 0.0f;
    this->terrain = TerrainType::DeepOcean;
}

int HexTile::GetTectonicPlateId() const {
    return this->tectonicPlateId;
}

void HexTile::SetTectonicPlateId(int id) {
    this->tectonicPlateId = id;
}

void HexTile::SetLand(bool land) {
    this->isLand = land;
}

bool HexTile::IsLand() const {
    return this->isLand;
}

float HexTile::GetHeight() const {
    return this->height;
}

void HexTile::SetHeight(float h) {
    this->height = h;
}

TerrainType HexTile::GetTerrain() const {
    return this->terrain;
}

void HexTile::SetTerrain(TerrainType t) {
    this->terrain = t;
}
