#include "TectonicsGenerator.h"

#include <algorithm>

TectonicsGenerator::TectonicsGenerator(int seed) : rng(seed){}

std::list<HexCoord> TectonicsGenerator::GenerateTectonicCenters(int count, HexGrid grid, float landRatio) {
    std::list<HexCoord> centers;
    std::list<int> centerIndices = rng.GenerateListBetween(0, grid.GetTotalCells() - 1, count);

    int landCenterCount = count * landRatio;

    std::vector<bool> landAssignments(count);
    for (int i = 0; i < landCenterCount; i++) {
        landAssignments[i] = true;
    }
    for (int i = landCenterCount; i < count; i++) {
        landAssignments[i] = false;
    }

    std::shuffle(landAssignments.begin(), landAssignments.end(), rng.GetEngine());

    int assignmentIndex = 0;
    for (int index : centerIndices) {
        HexCoord center = grid.GetHexCoord(index);
        bool isLand = landAssignments[assignmentIndex++];
        center.SetLand(isLand);
        center.SetTectonicPlateId(index);
        centers.push_back(center);
    }

    return centers;
}