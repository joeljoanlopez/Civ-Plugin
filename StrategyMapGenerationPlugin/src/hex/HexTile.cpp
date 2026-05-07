#include "hex/HexTile.h"

HexTile::HexTile() {
    this->tectonicPlateId = -1;
    this->isLand = false;
    this->height = 0.0f;
    this->terrain = 0;
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

int HexTile::GetTerrain() const {
    return this->terrain;
}

void HexTile::SetTerrain(int t) {
    this->terrain = t;
}
