#include "generation/TectonicsGenerator.h"
#include "api/MapGenerationAPI.h"
#include <queue>
#include <limits>

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

std::map<HexCoord, float> TectonicsGenerator::ComputeDistanceField(const HexGrid& grid) {
    std::map<HexCoord, int> distToWater;
    std::map<HexCoord, int> distToLand;

    std::queue<HexCoord> landFront;
    std::queue<HexCoord> waterFront;
    for (const auto&[coord, tile] : grid) {
        if (tile.IsLand()) {
            landFront.push(coord);
            distToLand[coord] = 0;
        }
        else {
            waterFront.push(coord);
            distToWater[coord] = 0;
        }
    }
    while (!waterFront.empty()) {
        HexCoord current = waterFront.front(); waterFront.pop();
        int d = distToWater[current];
        for (const HexCoord& neighbor : grid.GetNeighbors(current)) {
            if (grid.GetTileAt(neighbor).IsLand() && distToWater.find(neighbor) == distToWater.end()) {
                distToWater[neighbor] = d + 1;
                waterFront.push(neighbor);
            }
        }
    }
    while (!landFront.empty()) {
        HexCoord current = landFront.front(); landFront.pop();
        int d = distToLand[current];
        for (const HexCoord& neighbor : grid.GetNeighbors(current)) {
            if (!grid.GetTileAt(neighbor).IsLand() && distToLand.find(neighbor) == distToLand.end()) {
                distToLand[neighbor] = d + 1;
                landFront.push(neighbor);
            }
        }
    }

    int maxLandDist = 0;
    for (const auto&[coord, distance] : distToWater) {
        if (grid.GetTileAt(coord).IsLand())
            maxLandDist = std::max(maxLandDist, distance);
    }
    int maxWaterDist = 0;
    for (const auto&[coord, distance] : distToLand) {
        if (!grid.GetTileAt(coord).IsLand())
            maxWaterDist = std::max(maxWaterDist, distance);
    }

    std::map<HexCoord, float> distanceField;
    for (const auto&[coord, tile] : grid) {
        const HexCoord& distFieldCoord = coord;
        int raw = 0;
        if (tile.IsLand()) {
            if (distToWater.contains(distFieldCoord)) {
                raw  = distToWater.at(distFieldCoord);
            }
            if (maxLandDist > 0)
                distanceField[distFieldCoord] = static_cast<float>(raw) / static_cast<float>(maxLandDist);
            else
                distanceField[distFieldCoord] = 0.0f;
        } else {
            if (distToLand.contains(distFieldCoord)) {
                raw = distToLand.at(distFieldCoord);
            }
            if (maxWaterDist > 0)
                distanceField[distFieldCoord] = static_cast<float>(raw) / static_cast<float>(maxWaterDist);
            else
                distanceField[distFieldCoord] = 0.0f;
        }
    }

    return distanceField;
}

void TectonicsGenerator::ProcessTerrainMap(
    HexGrid& grid,
    const int noiseOctaves,
    const MapGenTerrainTypeDefinition* terrainTypes,
    const int terrainTypeCount,
    const TerrainNoiseSettings* noiseSettings
) const {
    const int totalCells = grid.GetTotalCells();

    TerrainNoiseSettings terrainNoiseSettings;
    if (noiseSettings != nullptr) {
        terrainNoiseSettings = *noiseSettings;
    } else {
        terrainNoiseSettings = MapGenGetTerrainNoiseSettings();
    }

    float landCoastHeight  =  std::numeric_limits<float>::max();
    float landInlandHeight = -std::numeric_limits<float>::max();
    float waterCoastHeight = -std::numeric_limits<float>::max();
    float waterDeepHeight  =  std::numeric_limits<float>::max();

    for (int i = 0; i < terrainTypeCount; ++i) {
        const MapGenTerrainTypeDefinition& terrainTypeDefinition = terrainTypes[i];
        if (terrainTypeDefinition.isWater) {
            if (terrainTypeDefinition.baseHeight > waterCoastHeight) waterCoastHeight = terrainTypeDefinition.baseHeight;
            if (terrainTypeDefinition.baseHeight < waterDeepHeight)  waterDeepHeight  = terrainTypeDefinition.baseHeight;
        } else {
            if (terrainTypeDefinition.baseHeight < landCoastHeight)  landCoastHeight  = terrainTypeDefinition.baseHeight;
            if (terrainTypeDefinition.baseHeight > landInlandHeight) landInlandHeight = terrainTypeDefinition.baseHeight;
        }
    }

    const auto distanceField = ComputeDistanceField(grid);

    for (int i = 0; i < totalCells; i++) {
        HexCoord coord = grid.GetCoordAt(i);
        HexTile& tile = grid.GetTileAt(coord);

        float distFactor = distanceField.at(coord);
        float t = distFactor * distFactor * (3.0f - 2.0f * distFactor); // smoothstep

        float baseHeight;
        if (tile.IsLand()) {
            baseHeight = landCoastHeight + t * (landInlandHeight - landCoastHeight);
        } else {
            baseHeight = waterCoastHeight + t * (waterDeepHeight - waterCoastHeight);
        }

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

        int terrainIndex = terrainTypeCount - 1;
        for (int j = 0; j < terrainTypeCount - 1; ++j) {
            if (finalHeight <= terrainTypes[j].maxHeight) {
                terrainIndex = j;
                break;
            }
        }
        tile.SetTerrain(terrainIndex);
    }
}
