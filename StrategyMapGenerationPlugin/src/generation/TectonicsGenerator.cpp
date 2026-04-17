#include "generation/TectonicsGenerator.h"
#include "api/MapGenerationAPI.h"
#include <algorithm>

TectonicsGenerator::TectonicsGenerator(int seed) : rng(seed), noiseGen(seed) {
}

std::list<TectonicsGenerator::PlateCenter> TectonicsGenerator::GenerateTectonicCenters(int count, const HexGrid& grid, float landRatio) {
    std::list<PlateCenter> centers;
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
        PlateCenter center;
        center.coord = grid.GetCoordAt(index);
        center.index = index;
        center.isLand = landAssignments[assignmentIndex++];
        centers.push_back(center);
    }

    return centers;
}

void TectonicsGenerator::AssignTectonicPlates(HexGrid& grid, const std::list<PlateCenter>& centers) {
    for (const PlateCenter& center : centers) {
        HexTile& tile = grid.GetTileAt(center.coord);
        tile.SetTectonicPlateId(center.index);
        tile.SetLand(center.isLand);
    }

    for (const auto& it : grid) {
        HexCoord coord = it.first;
        HexTile& tile = grid.GetTileAt(coord);

        if (tile.GetTectonicPlateId() != -1) {
            continue;
        }

        int nearestPlateId = -1;
        int minDistance = -1;

        for (const PlateCenter& center : centers) {
            int distance = coord.GetDistance(center.coord);
            if (minDistance == -1 || distance < minDistance) {
                minDistance = distance;
                nearestPlateId = center.index;
                tile.SetLand(center.isLand);
            }
        }

        tile.SetTectonicPlateId(nearestPlateId);
    }
}

void TectonicsGenerator::GenerateTectonicPlates(HexGrid& grid, int plateCount, float landRatio) {
    std::list<PlateCenter> centers = GenerateTectonicCenters(plateCount, grid, landRatio);
    AssignTectonicPlates(grid, centers);
}

void TectonicsGenerator::ProcessTerrainMap(
    HexGrid& grid,
    int noiseOctaves,
    const TerrainThresholds* thresholds,
    const TerrainBaseHeights* baseHeights
) const {
    int totalCells = grid.GetTotalCells();

    TerrainThresholds terrainThresholds;
    if (thresholds != nullptr) {
        terrainThresholds = *thresholds;
    } else {
        terrainThresholds = MapGenGetTerrainThresholds();
    }

    TerrainBaseHeights landDeterminationHeights;
    if (baseHeights != nullptr) {
        landDeterminationHeights = *baseHeights;
    } else {
        landDeterminationHeights = MapGenGetTerrainBaseHeights();
    }

    for (int i = 0; i < totalCells; i++) {
        float noiseScale = 0.1f;
        HexCoord coord = grid.GetCoordAt(i);
        HexTile& tile = grid.GetTileAt(coord);

        float baseHeight = tile.IsLand()
            ? landDeterminationHeights.landBaseHeight
            : landDeterminationHeights.waterBaseHeight;

        float noise = 0.0f;
        float amplitude = 1.0f;
        float frequency = 2.0f;
        float maxVal = 0.0f;

        float nx = static_cast<float>(coord.GetQ()) * noiseScale;
        float ny = static_cast<float>(coord.GetR()) * noiseScale;

        for(int o = 0; o < noiseOctaves; o++) {
            noise += noiseGen.Noise(nx * frequency, ny * frequency) * amplitude;
            maxVal += amplitude;
            amplitude *= 0.5f;
            frequency *= 2.0f;
        }
        noise /= maxVal;

        float finalHeight = baseHeight + (noise * 0.5f); // 0.5f és la força del soroll

        tile.SetHeight(finalHeight);

        if (finalHeight <= terrainThresholds.deepOceanMax) {
            tile.SetTerrain(TerrainType::DeepOcean);
        }
        else if (finalHeight <= terrainThresholds.waterMax) {
            tile.SetTerrain(TerrainType::Water);
        }
        else if (finalHeight <= terrainThresholds.coastMax) {
            tile.SetTerrain(TerrainType::Coast);
        }
        else if (finalHeight <= terrainThresholds.landMax) {
            tile.SetTerrain(TerrainType::Land);
        }
        else {
            tile.SetTerrain(TerrainType::Mountain);
        }
    }
}
