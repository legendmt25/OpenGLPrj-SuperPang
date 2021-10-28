#include "SphereRenderer.h"

SphereRenderer::SphereRenderer(Shader& shader, int squares, int layers)
    :squares(squares), layers(layers), shader(shader)
{
    this->initRenderData();
}

SphereRenderer::~SphereRenderer() {
    glDeleteVertexArrays(1, &this->QuadVAO);
}

void SphereRenderer::DrawSphere(Texture2D& texture, glm::vec3 position, glm::vec3 size = glm::vec3(10.0f), float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f)) {
    this->shader.Use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, size);

    this->shader.SetMatrix4("model", model);
    this->shader.SetVector3f("sprite3DColor", color);

    glActiveTexture(GL_TEXTURE0);
    texture.Bind();

    glBindVertexArray(this->QuadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 6 * squares * layers);
    glBindVertexArray(0);
}


void SphereRenderer::initRenderData() {
    const int a = 5;
    const float PI = glm::pi<float>();
    std::vector<float> vertices;
    float x, y, z;

    for (int j = 0; j < this->layers; j++) {
        float a1 = (float)j / this->layers * PI;
        float a2 = (float)(j + 1) / this->layers * PI;

        for (int i = 0; i <= this->squares; i++) {
            float b = (float)i / this->squares * 2.0f * PI;

            x = (float)(cos(b) * sin(a1));
            y = (float)(sin(b) * sin(a1));
            z = (float)(cos(a1));
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(b / (2.0f * PI));
            vertices.push_back(a1 / PI);

            x = (float)(cos(b) * sin(a2));
            y = (float)(sin(b) * sin(a2));
            z = (float)(cos(a2));
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(b / (2.0f * PI));
            vertices.push_back(a2 / PI);
        }
    }

    unsigned int VBO;
    glGenVertexArrays(1, &this->QuadVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(QuadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}