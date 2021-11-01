#include <iostream>
#include "Utility.h"

Direction VectorDirection(glm::vec2 target) {
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),	// up
        glm::vec2(1.0f, 0.0f),	// right
        glm::vec2(0.0f, -1.0f),	// down
        glm::vec2(-1.0f, 0.0f)	// left
    };

    float max = 0.0f;
    unsigned int best = -1;
    for (unsigned int i = 0; i < 4; ++i) {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (max < dot_product) {
            max = dot_product;
            best = i;
        }
    }
    return (Direction) best;
}

bool frameCount(const float& dt, float& frame, const float toFrame) {
    frame += dt;
    if (frame >= toFrame) {
        frame = 0.0f;
        return true;
    }
    return false;
}

std::vector<std::string> split(std::string str, std::string delimiter) {
    std::vector<std::string> res;
    size_t pos = 0;
    while ((pos = str.find(delimiter)) != std::string::npos) {
        res.push_back(str.substr(0, pos));
        str.erase(0, pos + delimiter.size());
    }
    if (str.size() > 0) {
        res.push_back(str);
    }
    return res;
}

std::string toLower(std::string str) {
    for (auto& c : str) {
        c = tolower(c);
    }
    return str;
}


Collision::Collision(bool collision, Direction direction, glm::vec2 difference)
    :collision(collision), direction(direction), difference(difference) {}