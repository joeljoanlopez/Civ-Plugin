#include "generation/TectonicsGenerator.h"
#include "api/MapGenerationAPI.h"

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

    rng.Shuffle(landAssignments);

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
    const TerrainBaseHeights* baseHeights,
    const TerrainNoiseSettings* noiseSettings
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

    TerrainNoiseSettings terrainNoiseSettings;
    if (noiseSettings != nullptr) {
        terrainNoiseSettings = *noiseSettings;
    } else {
        terrainNoiseSettings = MapGenGetTerrainNoiseSettings();
    }

    for (int i = 0; i < totalCells; i++) {
        HexCoord coord = grid.GetCoordAt(i);
        HexTile& tile = grid.GetTileAt(coord);

        float baseHeight = tile.IsLand()
            ? landDeterminationHeights.landBaseHeight
            : landDeterminationHeights.waterBaseHeight;

        float noise = 0.0f;
        float amplitude = terrainNoiseSettings.initialAmplitude;
        float frequency = terrainNoiseSettings.initialFrequency;
        float maxVal = 0.0f;

        float nx = static_cast<float>(coord.GetQ()) * terrainNoiseSettings.noiseScale;
        float ny = static_cast<float>(coord.GetR()) * terrainNoiseSettings.noiseScale;

        for(int o = 0; o < noiseOctaves; o++) {
            noise += noiseGen.Noise(nx * frequency, ny * frequency) * amplitude;
            maxVal += amplitude;
            amplitude *= terrainNoiseSettings.amplitudeDecay;
            frequency *= terrainNoiseSettings.frequencyMultiplier;
        }
        noise /= maxVal;

        float finalHeight = baseHeight + (noise * terrainNoiseSettings.noiseStrength);

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
