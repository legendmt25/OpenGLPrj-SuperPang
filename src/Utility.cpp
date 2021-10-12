#include <Utility.h>

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

Collision::Collision(bool collision, Direction direction, glm::vec2 difference)
    :collision(collision), direction(direction), difference(difference) {}