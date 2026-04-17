#include "api/MapGenerationAPI.h"
#include <cstdio>

int main() {
    MapGenMapData map = {};
    int ok = MapGenGenerateMap(50, 50, 12345, 6, 0.4f, 3, nullptr, nullptr, nullptr, &map);
    if (!ok) {
        fprintf(stderr, "MapGenGenerateMap failed\n");
        return 1;
    }
    for (int i = 0; i < map.tileCount; ++i) {
        const MapGenTileData& tile = map.tiles[i];
        fprintf(stdout, "%d,%d,%d,%d,%.8f,%d\n",
            tile.q, tile.r, tile.tectonicPlateId, tile.isLand, static_cast<double>(tile.height), tile.terrain);
    }
    MapGenFreeMap(&map);
    return 0;
}
