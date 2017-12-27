#ifndef _RAINDROP_H
#define _RAINDROP_H

#include "Shader.hpp"

class RainDrop
{
public:
    RainDrop(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height);
    void Draw(gps::Shader shader);
    void applyWeight();

private:
    GLfloat vertices[12];   // 4 distinct vertices * 3 dimensions
    GLfloat indices[6];
    const GLfloat weight = 1.0f;
    GLuint verticesVBO;
    GLuint verticesEBO;
    GLuint objectVAO;

    void initObject();
};

#endif /**/
