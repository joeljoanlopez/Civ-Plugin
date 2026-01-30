#include "LineGenerator_Wrapper.h"

EXPORT_API int LineGenerator_GenerateLine(
    float startX, float startY, float startZ,
    int count,
    float spacing,
    char direction,
    float* outArray
) {
    CoreVector3 startPos(startX, startY, startZ);
    std::vector<CoreVector3> linePositions = GenerateLine(startPos, count, spacing, direction);

    if (outArray != nullptr) {
        for (int i = 0; i < linePositions.size(); i++) {
            outArray[i * 3 + 0] = linePositions[i].x;
            outArray[i * 3 + 1] = linePositions[i].y;
            outArray[i * 3 + 2] = linePositions[i].z;
        }
    }

    return linePositions.size();
}