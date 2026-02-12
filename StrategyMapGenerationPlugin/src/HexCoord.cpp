#include "HexCoord.h"

HexCoord::HexCoord() {
    this->q = 0;
    this->r = 0;
    this->tectonicPlateId = -1;
    this->isLand = false;
}

HexCoord::HexCoord(const int q, const int r){
    this->q = q;
    this->r = r;
    this->tectonicPlateId = -1;
    this->isLand = false;
}

int HexCoord::GetQ() const {
    return this->q;
}

int HexCoord::GetR() const {
    return this->r;
}

int HexCoord::GetS() const {
    return -this->q - this->r;
}

int HexCoord::GetTectonicPlateId() const {
    return this->tectonicPlateId;
}

void HexCoord::SetTectonicPlateId(int id) {
    this->tectonicPlateId = id;
}

void HexCoord::SetLand(bool land) {
    this->isLand = land;
}

bool HexCoord::IsLand() const {
    return this->isLand;
}

int HexCoord::GetDistance(HexCoord other) {
    int dq = std::abs(this->q - other.GetQ());
    int dr = std::abs(this->r - other.GetR());
    int ds = std::abs(this->GetS() - other.GetS());
    return std::max(std::max(dq, dr), ds);
}

bool HexCoord::operator==(const HexCoord &other) const {
    return other.GetQ() == this->q && other.GetR() == this->r;
}

bool HexCoord::operator<(const HexCoord& other) const
{
    if (this->q != other.GetQ()) return this->q < other.GetQ();
    if (this->r != other.GetR()) return this->r < other.GetR();
    return this->GetS() < other.GetS();
}

