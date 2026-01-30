#include "HexCoord.h"

HexCoord::HexCoord(const int q, const int r){
    this->q = q;
    this->r = r;
    this->s = -q - r;
}

int HexCoord::GetQ() const {
    return this->q;
}

int HexCoord::GetR() const {
    return this->r;
}

int HexCoord::GetS() const {
    return this->s;
}

bool HexCoord::operator==(const HexCoord& other) const
{
    return other.q == this->q && other.r == this->r;
}