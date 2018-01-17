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
    this->windDirectionAngle = 0.0f;
    this->windPower = 0.0f;
    this->windActive = false;
    this->rainDropAngle = 0.0f;
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
    glm::vec3 aux;
    glm::mat4 windDirection;
    shader.useShaderProgram();

    std::list<ModelPos>::iterator it;
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");

    /*
    For each drop:
    1. draw at its specific position, without any wind effect
    2. translate and rotate according to wind
    (in order to rotate the rain drops with the wind direction, we need a vector pointing to the right side of the direction)
    */

    for(it=rain_drops.begin(); it != rain_drops.end(); it++)
    {
        model = glm::mat4(1.0f);
        if(windActive)
        {
            windDirection = glm::mat4(0.0f);
            windDirection[0].x = windPower;
            windDirection = glm::rotate(windDirection, glm::radians(windDirectionAngle), glm::vec3(0.0f, 1.0f, 0.0f));

            aux = glm::normalize(glm::cross(glm::vec3(windDirection[0].x, 0.0f, windDirection[2].z), glm::vec3(0.0f, 1.0f, 0.0f)));
            model = glm::rotate(model, glm::radians(45.0f), aux);
            model = glm::translate(model, glm::vec3(windDirection[0].x * 5.0f, 0.0f, windDirection[2].z * 5.0f));
        }
        model = glm::translate(model, it->offset);
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

void RainManager::rotateWindDirection(GLfloat angle)
{
    windDirectionAngle += angle;
}

void RainManager::addWindPower(GLfloat power)
{
    windPower = std::min(std::max(windPower + power, 0.0f), 1.0f);

    // update the rain drop angle
    // 0.0f --> 0 degrees, 1.0f --> 45 degrees
    rainDropAngle = windPower * 45.0f;
}

void RainManager::setWindActive(GLboolean value)
{
    windActive = value;
}
