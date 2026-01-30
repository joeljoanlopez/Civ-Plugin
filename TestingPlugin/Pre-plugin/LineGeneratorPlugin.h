#pragma once

#include <vector>

struct CoreVector3 {
    float x;
    float y;
    float z;

    CoreVector3(float x, float y, float z){
        this->x = x;
        this->y = y;
        this->z = z;
    }

    CoreVector3() {
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }
};

std::vector<CoreVector3> GenerateLine (CoreVector3 startPos, int count, float spacing, char direction);