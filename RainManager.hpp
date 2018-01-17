#ifndef _RAINDROP_H
#define _RAINDROP_H

#include "Model3D.hpp"
#include <list>
#include <glm/glm.hpp>//core glm functionality
#include <glm/gtc/matrix_transform.hpp>//glm extension for generating common transformation matrices
#include <glm/gtc/type_ptr.hpp>

struct ModelPos
{
    gps::Model3D model;
    glm::vec3 offset;
};

class RainManager
{
public:
    RainManager(GLfloat disappear_threshold, GLfloat weight);
    void Draw(gps::Shader shader);
    void applyWeight();
    void addWeight(GLfloat weight);
    void increaseDensity();
    void decreaseDensity();
    void clear();
    GLboolean is_raining();
    void setRaining(GLboolean value);
    void rotateWindDirection(GLfloat angle);
    void addWindPower(GLfloat power);
    void setWindActive(GLboolean value);

private:
    GLfloat disappear_threshold;
    GLfloat weight;
    std::list<ModelPos> rain_drops;
    GLboolean raining;
    GLuint density;

    GLfloat minWeight, maxWeight;

    GLint xRadius, yRadius, zRadius;
    GLint minSpawnY;

    GLfloat windDirectionAngle;
    GLfloat windPower;
    GLboolean windActive;
    GLfloat rainDropAngle;
    void generateDrops(GLuint count);

};

#endif /**/
