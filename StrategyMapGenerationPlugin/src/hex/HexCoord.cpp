#include "hex/HexCoord.h"

HexCoord::HexCoord() {
    this->q = 0;
    this->r = 0;
}

HexCoord::HexCoord(const int q, const int r){
    this->q = q;
    this->r = r;
}

int HexCoord::GetDistance(HexCoord other) const {
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



