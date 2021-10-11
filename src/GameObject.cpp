#include "GameObject.h"

GameObject::GameObject()
    : GameObject(glm::vec2(0.0f), glm::vec2(1.0f), Texture2D()) {}

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D texture, glm::vec3 color, glm::vec2 velocity)
    : Position(pos), Size(size), Velocity(velocity), Color(color), Rotation(0.0f), Texture(texture), IsSolid(false), Destroyed(false) { }

void GameObject::Draw(SpriteRenderer& renderer)
{
    renderer.DrawSprite(this->Texture, this->Position, this->Size, this->Rotation, this->Color);
}

Collision GameObject::checkCollision(GameObject& obj) {
    bool collisionX = this->Position.x + this->Size.x >= obj.Position.x &&
                      obj.Position.x + obj.Size.x >= this->Position.x;
    bool collisionY = this->Position.y + this->Size.y >= obj.Position.y &&
                      obj.Position.y + obj.Size.y >= this->Position.y;
    bool collision = collisionX && collisionY;

    if (collision) {
        return std::make_tuple(true, UP, glm::vec2(0.0f));
    }
    else {
        return std::make_tuple(false, Direction::UP, glm::vec2(0.0f));
    }
}

glm::vec2& GameObject::Move(float dt, unsigned int window_width, unsigned int window_height) {
    return this->Position;
}

void GameObject::Reset() {
    Destroyed = false;
}
