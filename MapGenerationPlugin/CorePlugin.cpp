#include "CorePlugin.h"

std::vector<CoreVector3> GenerateLine (CoreVector3 startPos, int count, float spacing, char direction)
{
    std::vector<CoreVector3> linePositions;
    for (int i = 0; i < count; i++) {
        CoreVector3 newPos = CoreVector3(
            startPos.x + direction == 'x' ? i * spacing : 0,
            startPos.y + direction == 'y' ? i * spacing : 0,
            startPos.z + direction == 'z' ? i * spacing : 0
        );
        linePositions.push_back(newPos);
    }

    return linePositions;
}