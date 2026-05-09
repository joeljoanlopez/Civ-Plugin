#include "generation/TectonicsGenerator.h"
#include "api/MapGenerationAPI.h"
#include <queue>
#include <limits>
#include <unordered_map>
#include <algorithm>
#include <cmath>

TectonicsGenerator::TectonicsGenerator(int seed) : rng(seed), noiseGen(seed) {
}

std::list<TectonicsGenerator::PlateCenter> TectonicsGenerator::GenerateTectonicCenters(int count, const HexGrid& grid, float landRatio) {
    std::list<PlateCenter> centers;
    std::list<int> centerIndices = rng.GenerateListBetween(0, grid.GetTotalCells() - 1, count);

    const int landCenterCount = static_cast<int>(std::min(
        static_cast<float>(count) * landRatio,
        static_cast<float>(count)
    ));

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

std::unordered_map<HexCoord, float> TectonicsGenerator::ComputeDistanceField(const HexGrid &grid) {
    std::unordered_map<HexCoord, int> distToWater;
    std::unordered_map<HexCoord, int> distToLand;

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

    std::unordered_map<HexCoord, float> distanceField;
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

static float ClampF(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

static bool HasClimateRange(const MapGenTerrainTypeDefinition& t) {
    return t.maxTemperature > 0.0f || t.maxMoisture > 0.0f;
}

void TectonicsGenerator::ProcessTerrainMap(
    HexGrid& grid,
    const int noiseOctaves,
    const MapGenTerrainTypeDefinition* terrainTypes,
    const int terrainTypeCount,
    const TerrainNoiseSettings* noiseSettings,
    const MapGenClimateSettings* climateSettings
) const {
    const int totalCells = grid.GetTotalCells();

    TerrainNoiseSettings terrainNoiseSettings;
    if (noiseSettings != nullptr) {
        terrainNoiseSettings = *noiseSettings;
    } else {
        terrainNoiseSettings = MapGenGetTerrainNoiseSettings();
    }

    MapGenClimateSettings climate;
    if (climateSettings != nullptr) {
        climate = *climateSettings;
    } else {
        climate = MapGenGetDefaultClimateSettings();
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

    const float gridHeight = static_cast<float>(grid.GetHeight() - 1);

    bool anyLandHasClimate = false;
    for (int i = 0; i < terrainTypeCount; ++i) {
        if (!terrainTypes[i].isWater && HasClimateRange(terrainTypes[i])) {
            anyLandHasClimate = true;
            break;
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

        float temperature = 0.0f;
        float moisture    = 0.0f;

        if (anyLandHasClimate) {
            float normalizedRow = 0.5f;
            if (gridHeight > 0.0f) {
                normalizedRow = static_cast<float>(coord.GetR()) / gridHeight;
            }
            float latFactor = 1.0f - std::abs(normalizedRow - climate.equatorNormalizedRow) * 2.0f;
            latFactor = ClampF(latFactor, 0.0f, 1.0f);

            float elevAboveSea = ClampF(finalHeight, 0.0f, 1.0f);
            float tempNoise = noiseGen.Noise(nx * 0.4f + 100.0f, ny * 0.4f) * climate.temperatureNoiseStrength;
            temperature = ClampF(latFactor - elevAboveSea * climate.elevationTempPenalty + tempNoise, 0.0f, 1.0f);

            float moistNoise = noiseGen.Noise(nx * 0.35f + 200.0f, ny * 0.35f + 100.0f) * climate.moistureNoiseStrength;
            moisture = ClampF(1.0f - distFactor + moistNoise, 0.0f, 1.0f);
        }

        tile.SetTemperature(temperature);
        tile.SetMoisture(moisture);

        int terrainIndex = terrainTypeCount - 1;

        if (tile.IsLand() && anyLandHasClimate) {
            float bestDist = std::numeric_limits<float>::max();
            int bestIdx = -1;

            for (int j = 0; j < terrainTypeCount; ++j) {
                const MapGenTerrainTypeDefinition& td = terrainTypes[j];
                if (td.isWater) continue;
                if (finalHeight > td.maxHeight) continue;
                if (!HasClimateRange(td)) continue;

                float tempCenter = (td.minTemperature + td.maxTemperature) * 0.5f;
                float moistCenter = (td.minMoisture + td.maxMoisture) * 0.5f;
                float dt = temperature - tempCenter;
                float dm = moisture - moistCenter;
                float dist = dt * dt + dm * dm;

                if (dist < bestDist) {
                    bestDist = dist;
                    bestIdx = j;
                }
            }

            if (bestIdx >= 0) {
                terrainIndex = bestIdx;
            } else {
                for (int j = 0; j < terrainTypeCount - 1; ++j) {
                    if (finalHeight <= terrainTypes[j].maxHeight) {
                        terrainIndex = j;
                        break;
                    }
                }
            }
        } else {
            for (int j = 0; j < terrainTypeCount - 1; ++j) {
                if (finalHeight <= terrainTypes[j].maxHeight) {
                    terrainIndex = j;
                    break;
                }
            }
        }

        tile.SetTerrain(terrainIndex);
    }
}
