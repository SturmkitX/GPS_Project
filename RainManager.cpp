#include "RainManager.hpp"

RainManager::RainManager(GLfloat disappear_threshold, GLfloat weight)
{
    this->disappear_threshold = disappear_threshold;
    this->weight = weight;
    this->raining = false;
    this->density = 0;
    this->xRadius = 500;
    this->yRadius = 40;
    this->zRadius = 500;
    this->minSpawnY = 120;
}

void RainManager::applyWeight()
{
    std::list<ModelPos>::iterator it;
    for(it=rain_drops.begin(); it != rain_drops.end(); it++)
    {
        it->offset.y -= weight;    // the object goes down
        if(it->offset.y < disappear_threshold)
        {
            // printf("Reordered node\n");
            it->offset.y = (GLfloat)(std::rand() % yRadius + minSpawnY);
        }
    }
}

void RainManager::Draw(gps::Shader shader)
{
    GLuint modelLoc;
    glm::mat4 model;
    shader.useShaderProgram();

    std::list<ModelPos>::iterator it;

    for(it=rain_drops.begin(); it != rain_drops.end(); it++)
    {
        modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
        model = glm::translate(glm::mat4(1.0f), it->offset);
        model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        it->model.Draw(shader);
    }

}

void RainManager::generateDrops(GLuint count)
{
    for(int i=0; i<count; i++)
    {
        ModelPos aux;
        aux.model = gps::Model3D("objects/raindrop/jeg_lowpoly.obj", "objects/raindrop/");
        glm::vec3 pos;
        pos.x = GLfloat(std::rand() % (2*xRadius) - xRadius);
        pos.y = (GLfloat)(std::rand() % yRadius + minSpawnY);
        pos.z = GLfloat(std::rand() % (2*zRadius) - zRadius);
        aux.offset = pos;

        rain_drops.push_back(aux);
    }
}

void RainManager::clear()
{
    rain_drops.clear();
}

GLboolean RainManager::is_raining()
{
    return raining;
}

void RainManager::setRaining(GLboolean value)
{
    raining = value;
}

void RainManager::increaseDensity()
{
    generateDrops(32);
    density += 32;
}

void RainManager::decreaseDensity()
{
    if(density)
    {
        std::list<ModelPos>::iterator it = rain_drops.begin();
        std::advance(it, 32);
        rain_drops.erase(rain_drops.begin(), it);
        density -= 32;
    }
}
