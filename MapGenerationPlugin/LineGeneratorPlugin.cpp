#include "LineGeneratorPlugin.h"

std::vector<CoreVector3> GenerateLine (CoreVector3 startPos, int count, float spacing, char direction)
{
    std::vector<CoreVector3> linePositions;
    for (int i = 0; i < count; i++) {
        float offsetX = (direction == 'x' || direction == 'X') ? i * spacing : 0.0f;
        float offsetY = (direction == 'y' || direction == 'Y') ? i * spacing : 0.0f;
        float offsetZ = (direction == 'z' || direction == 'Z') ? i * spacing : 0.0f;

        CoreVector3 newPos = CoreVector3(
            startPos.x + offsetX,
            startPos.y + offsetY,
            startPos.z + offsetZ
        );
        linePositions.push_back(newPos);
    }

    return linePositions;
}