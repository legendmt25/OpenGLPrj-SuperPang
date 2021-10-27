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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void SphereRenderer::initRenderData() {
    const int a = 5;
    const float PI = glm::pi<float>();
    std::vector<float> vertices;

    float angleHStep = 2 * PI / squares;
    float angleVStep = 4 * PI / layers;
    float angleH = 0, angleV = 0;

    float s, t;
    for (int i = 0; i < layers + 4; i++)
    {
        angleV = PI / 2 - i * angleVStep;

        for (int j = 0; j <= squares; j++)
        {
            vertices.push_back(cos(angleV) * cos(angleH)); //x
            vertices.push_back(cos(angleV) * sin(angleH)); //y
            vertices.push_back(sin(angleV)); //z

            s = (float)j / squares;
            t = (float)i / layers;
            vertices.push_back(s);
            vertices.push_back(t);
            angleH += angleHStep;
        }
    }

    std::vector<int> indices;
    int topLeft, bottomLeft;
    for (int i = 0; i < layers; i++)
    {
        topLeft = i * (squares + 1);
        bottomLeft = topLeft + squares + 1;

        for (int j = 0; j < squares; j++, topLeft++, bottomLeft++)
        {
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topLeft + 1);

            indices.push_back(topLeft + 1);
            indices.push_back(bottomLeft);
            indices.push_back(bottomLeft + 1);
        }
    }

    this->indicesCount = indices.size();

    unsigned int VBO;
    glGenVertexArrays(1, &this->QuadVAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &this->EBO);

    glBindVertexArray(this->QuadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size(), &indices[0], GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, a * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, a * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}