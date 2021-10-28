#include "GameObject.h"

GameObject::GameObject()
    : GameObject(glm::vec3(0.0f), glm::vec3(1.0f), Texture2D()) {}

GameObject::GameObject(glm::vec3 pos, glm::vec3 size, Texture2D texture, glm::vec3 color, glm::vec3 velocity)
    : Position(pos), Size(size), Velocity(velocity), Color(color), Rotation(0.0f), Texture(texture), IsSolid(false), Destroyed(false) { }

void GameObject::Draw(Sprite3DRenderer& renderer)
{
    renderer.DrawSprite(this->Texture, this->Position, this->Size, this->Rotation, this->Color);
}

Collision GameObject::checkCollision(GameObject& obj) {
    bool collisionX = this->Position.x + this->Size.x / 2.0f >= obj.Position.x - obj.Size.x / 2.0f &&
                      obj.Position.x + obj.Size.x / 2.0f >= this->Position.x - this->Size.x / 2.0f;
    bool collisionY = this->Position.y + this->Size.y / 2.0f >= obj.Position.y - obj.Size.y / 2.0f &&
                      obj.Position.y + obj.Size.y / 2.0f >= this->Position.y - this->Size.y / 2.0f;
    bool collision = collisionX && collisionY;

    return Collision(collision);
}

glm::vec3& GameObject::Move(float dt, unsigned int window_width, unsigned int window_height) {
    return this->Position;
}

void GameObject::Reset() {
    this->Destroyed = false;
}

void GameObject::ProcessInput(float dt, unsigned int window_width, unsigned int window_height) {
    return;
}
