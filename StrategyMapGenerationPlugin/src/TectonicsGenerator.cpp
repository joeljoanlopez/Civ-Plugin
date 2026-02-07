#include "TectonicsGenerator.h"

TectonicsGenerator::TectonicsGenerator(int seed) : rng(seed){}

std::list<HexCoord> TectonicsGenerator::GenerateTectonicCenters(int count, HexGrid grid) {
    std::list<HexCoord> centers;
    std::list<int> centerIndices = rng.GenerateListBetween(0, grid.GetTotalCells() - 1, count);

    for (int index : centerIndices) {
        HexCoord center = grid.GetHexCoord(index);
        center.SetTectonicPlateId(index);
        centers.push_back(center);
    }

    return centers;
}