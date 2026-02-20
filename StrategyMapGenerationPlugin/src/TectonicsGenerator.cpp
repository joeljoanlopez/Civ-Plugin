#include "TectonicsGenerator.h"

#include <algorithm>

TectonicsGenerator::TectonicsGenerator(int seed) : rng(seed), noiseGen(seed) {
}

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

void TectonicsGenerator::ProcessTerrainMap(HexGrid& grid, int noiseOctaves) {
    int totalCells = grid.GetTotalCells();
    float noiseScale = 0.1f;

    for (int i = 0; i < totalCells; i++) {
        HexCoord& cell = grid.GetMutableHexCoord(i);

        float baseHeight = cell.IsLand() ? 0.5f : -0.2f;

        float noise = 0.0f;
        float amplitude = 1.0f;
        float frequency = 1.0f;
        float maxVal = 0.0f;

        float nx = static_cast<float>(cell.GetQ()) * noiseScale;
        float ny = static_cast<float>(cell.GetR()) * noiseScale;

        for(int o = 0; o < noiseOctaves; o++) {
            noise += noiseGen.Noise(nx * frequency, ny * frequency) * amplitude;
            maxVal += amplitude;
            amplitude *= 0.5f;
            frequency *= 2.0f;
        }
        noise /= maxVal;

        float finalHeight = baseHeight + (noise * 0.5f); // 0.5f és la força del soroll

        cell.SetHeight(finalHeight);

        // 5. Aplicar Thresholds (Classificació)
        if (finalHeight < 0.2f) {
            cell.SetTerrain(TerrainType::DeepOcean);
        }
        else if (finalHeight < 0.3f) {
            cell.SetTerrain(TerrainType::Water);
        }
        else if (finalHeight < 0.45f) {
            cell.SetTerrain(TerrainType::Coast);
        }
        else if (finalHeight > 0.75f) {
            cell.SetTerrain(TerrainType::Mountain);
        }
        else {
            cell.SetTerrain(TerrainType::Land);
        }
    }
}