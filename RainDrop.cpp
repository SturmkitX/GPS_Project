#include "RainDrop.hpp"

RainDrop::RainDrop(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height)
{
    // register the vertices in counter-clockwise order
    this->vertices[0] = x; this->vertices[1] = y; this->vertices[2] = z;
    this->vertices[3] = x + width; this->vertices[4] = y; this->vertices[5] = z;
    this->vertices[6] = x; this->vertices[7] = y + height; this->vertices[8] = z;
    this->vertices[9] = x + width; this->vertices[10] = y + height; this->vertices[11] = z;

    this->indices[0] = 0; this->indices[1] = 1; this->indices[2] = 2;
    this->indices[3] = 1; this->indices[4] = 3; this->indices[5] = 2;

    initObject();
}

void RainDrop::initObject()
{
    glGenVertexArrays(1, &objectVAO);
    glBindVertexArray(objectVAO);

    glGenBuffers(1, &verticesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &verticesEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, verticesEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //vertex position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void RainDrop::Draw(gps::Shader shader)
{
    glUseProgram(shader.shaderProgram);
    glBindVertexArray(objectVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void RainDrop::applyWeight()
{
    vertices[1] -= weight;
    vertices[4] -= weight;
    vertices[7] -= weight;
    vertices[10] -= weight;
}
