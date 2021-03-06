//
//  main.cpp
//  OpenGL Shadows
//
//  Created by CGIS on 05/12/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC

#include <iostream>
#include <glm/glm.hpp>//core glm functionality
#include <glm/gtc/matrix_transform.hpp>//glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include "Shader.hpp"
#include "Camera.hpp"
#define TINYOBJLOADER_IMPLEMENTATION

#include "Model3D.hpp"
#include "Mesh.hpp"
#include "SkyBox.hpp"
#include "RainManager.hpp"
#include <ctime>

int glWindowWidth = 640;
int glWindowHeight = 480;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const GLuint SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat3 lightDirMatrix;
GLuint lightDirMatrixLoc;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

gps::Camera myCamera(glm::vec3(0.0f, 1.0f, 2.5f), glm::vec3(0.0f, 0.0f, 0.0f));
GLfloat cameraSpeed = 1.0f;

bool pressedKeys[1024];
GLfloat angle;
GLfloat lightAngle;

gps::Model3D myModel;
gps::Model3D ground;
gps::Model3D lightCube;
gps::Model3D island;

gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader depthMapShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

GLfloat delta_x = 0.0f, delta_y = 0.0f;
GLfloat start_x, start_y;
GLboolean firstMouseMovement = true;
GLdouble last_time, act_time;
GLfloat lightDir_offset = 0.0f;

GLfloat fogDensity = 0.005f;
GLfloat alphaFactor = 0.5f;

GLfloat groundDeltaX = 0.0f, groundDeltaY = 0.0f;
GLfloat groundMaxDeltaLeft = -10.0f, groundMaxDeltaRight = 10.0f;
GLfloat groundMaxDeltaDown = -5.0f, groundMaxDeltaUp = 10.0f;
GLint groundDirectionX = 1, groundDirectionY = 1;  // -1 = left, 1 = right

RainManager rain(-50.0f, 3.0f);

gps::Model3D docks;
gps::Model3D barrel;
gps::Model3D lantern;
gps::Model3D bottle;
gps::Model3D lightCube2;
gps::Model3D princess;
gps::Model3D diamond;

glm::vec3 lightDir2(-210.0f, 20.0f, -150.0f);
GLfloat lightAngle2 = 0.0f;
GLfloat diffuseStrength = 1.0f;
glm::vec3 lightDir3(-200.0f, 30.0f, -180.0f);
GLfloat lightAngle3 = 0.0f;

GLboolean drawWireframe = false;
GLfloat diamondRotation = 0.0f;

GLFWmonitor* monitor;
const GLFWvidmode* vidMode;
GLboolean isFullscreen = false;

gps::Model3D helicopter_base;
gps::Model3D helicopter_blades1;
gps::Model3D helicopter_blades2;
glm::vec3 heli_base_origpos(-0.4f, 284.3f, 649.7f);
glm::vec3 heli_blades1_origpos(-0.47f, 301.4f, 624.8f);
glm::vec3 heli_blades2_origpos(4.8f, 302.8f, 713.7f);
GLfloat heli_base_offset = 0.0f;
GLfloat heli_blades_angle = 0.0f;

GLfloat autoMouseAngle = 0.0f;
GLint autoMouseDirection = 1;
GLboolean mouseAnimationEnabled = false;

gps::Model3D sun;
gps::Model3D ground_extension;

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	myCustomShader.useShaderProgram();

	//set projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	//send matrix data to shader
	GLint projLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	lightShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//set Viewport transform
	glViewport(0, 0, retina_width, retina_height);

    // reset first mouse movement
    firstMouseMovement = true;
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouseMovement)
    {
        start_x = (GLfloat) xpos;
        start_y = (GLfloat) ypos;
        firstMouseMovement = false;
    }

    delta_x = (GLfloat)xpos - start_x;
    delta_y = (GLfloat)ypos - start_y;
    // std :: cout << delta_x << ' ' << delta_y << '\n';

    float sensitivity = 0.05f;
    delta_x *= sensitivity;
    delta_y *= sensitivity;

    if (delta_y > 89.0f){
        delta_y = 89.0f;
    }
    if (delta_y < -89.0f){
        delta_y = -89.0f;
    }

    myCamera.rotate(-delta_y, delta_x);

}

void processMovement()
{

	if (pressedKeys[GLFW_KEY_Q]) {
		angle += 1.0f;
		if (angle > 360.0f)
			angle -= 360.0f;
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angle -= 1.0f;
		if (angle < 0.0f)
			angle += 360.0f;
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_J]) {

		lightAngle += 0.3f;
		if (lightAngle > 360.0f)
			lightAngle -= 360.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle -= 0.3f;
		if (lightAngle < 0.0f)
			lightAngle += 360.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}

    if(pressedKeys[GLFW_KEY_KP_ADD])
    {
        fogDensity = glm::min(fogDensity + 0.0001f, 1.0f);

    }

    if(pressedKeys[GLFW_KEY_KP_SUBTRACT])
    {
        fogDensity = glm::max(fogDensity - 0.0001f, 0.0f);
    }

    if(pressedKeys[GLFW_KEY_1])
    {
        rain.setRaining(true);
    }

    if(pressedKeys[GLFW_KEY_2])
    {
        rain.setRaining(false);
    }

    if(pressedKeys[GLFW_KEY_3])
    {
        rain.increaseDensity();
    }

    if(pressedKeys[GLFW_KEY_4])
    {
        rain.decreaseDensity();
    }

    if(pressedKeys[GLFW_KEY_5])
    {
        drawWireframe = true;
    }

    if(pressedKeys[GLFW_KEY_6])
    {
        drawWireframe = false;
    }

    if(pressedKeys[GLFW_KEY_LEFT_BRACKET])
    {
        alphaFactor = std::max(0.0f, alphaFactor - 0.02f);
    }

    if(pressedKeys[GLFW_KEY_RIGHT_BRACKET])
    {
        alphaFactor = std::min(1.0f, alphaFactor + 0.02f);
    }

	if(pressedKeys[GLFW_KEY_F] && !isFullscreen)
	{
		// glfwSetWindowSize(glWindow, vidMode->width, vidMode->height);
		glfwSetWindowMonitor(glWindow, monitor, 0, 0, vidMode->width, vidMode->height, vidMode->refreshRate);
		isFullscreen = true;
	}

	if(pressedKeys[GLFW_KEY_G] && isFullscreen)
	{
		// glfwSetWindowSize(glWindow, glWindowWidth, glWindowHeight);
		glfwSetWindowMonitor(glWindow, NULL, vidMode->width / 4, vidMode->height / 4, glWindowWidth, glWindowHeight, 0);
		isFullscreen = false;
	}

	if(pressedKeys[GLFW_KEY_9])
	{
		rain.setWindActive(true);
	}

	if(pressedKeys[GLFW_KEY_0])
	{
		rain.setWindActive(false);
	}

	if(pressedKeys[GLFW_KEY_RIGHT])
	{
		rain.rotateWindDirection(1.0f);
	}

	if(pressedKeys[GLFW_KEY_LEFT])
	{
		rain.rotateWindDirection(-1.0f);
	}

	if(pressedKeys[GLFW_KEY_UP])
	{
		rain.addWindPower(0.05f);
	}

	if(pressedKeys[GLFW_KEY_DOWN])
	{
		rain.addWindPower(-0.05f);
	}

	if(pressedKeys[GLFW_KEY_7])
	{
		mouseAnimationEnabled = true;
	}

	if(pressedKeys[GLFW_KEY_8])
	{
		mouseAnimationEnabled = false;
	}

	if(pressedKeys[GLFW_KEY_Z])
	{
		myModel = gps::Model3D("objects/stone/stone.obj", "objects/stone/");
	}

	if(pressedKeys[GLFW_KEY_X])
	{
		myModel = gps::Model3D("objects/stone/stone_lowpoly.obj", "objects/stone/");
	}

	if(pressedKeys[GLFW_KEY_C])
	{
		rain.addWeight(-0.5f);
	}

	if(pressedKeys[GLFW_KEY_V])
	{
		rain.addWeight(0.5f);
	}
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	//for Mac OS X
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwMakeContextCurrent(glWindow);

	glfwWindowHint(GLFW_SAMPLES, 4);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
    glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	//glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initFBOs()
{
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);

	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix()
{
	const GLfloat near_plane = 1.0f, far_plane = 10.0f;
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);

	glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
	glm::mat4 lightView = glm::lookAt(lightDirTr, myCamera.getCameraTarget(), glm::vec3(0.0f, 1.0f, 0.0f));

	return lightProjection * lightView;
}

glm::mat4 computeLightSpaceTrMatrix2()
{
	const GLfloat near_plane = 1.0f, far_plane = 10.0f;
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);

	glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle2), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::vec4(lightDir2, 1.0f));
	glm::mat4 lightView = glm::lookAt(lightDirTr, myCamera.getCameraTarget(), glm::vec3(0.0f, 1.0f, 0.0f));

	return lightProjection * lightView;
}

void initModels()
{
	myModel = gps::Model3D("objects/stone/stone.obj", "objects/stone/");
	ground = gps::Model3D("objects/ground/ground.obj", "objects/ground/");
	lightCube = gps::Model3D("objects/cube/cube.obj", "objects/cube/");
	island = gps::Model3D("objects/tropical_island/tropical_island2.obj", "objects/tropical_island/");
    docks = gps::Model3D("objects/old_house/old_house.obj", "objects/old_house/");
    barrel = gps::Model3D("objects/barrel/barrel.obj", "objects/barrel/");
    lantern = gps::Model3D("objects/lantern/lantern.obj", "objects/lantern/");
    bottle = gps::Model3D("objects/bottle/bottle.obj", "objects/bottle/");
    lightCube2 = gps::Model3D("objects/cube/cube.obj", "objects/cube/");
    princess = gps::Model3D("objects/princess_leia/leia.obj", "objects/princess_leia/");
    diamond = gps::Model3D("objects/diamond/Diamond.obj", "objects/diamond/");
	helicopter_base = gps::Model3D("objects/helicopter/helicopter_base.obj", "objects/helicopter/");
	helicopter_blades1 = gps::Model3D("objects/helicopter/helicopter_blades1.obj", "objects/helicopter/");
	helicopter_blades2 = gps::Model3D("objects/helicopter/helicopter_blades2.obj", "objects/helicopter/");
	sun = gps::Model3D("objects/sun/sun.obj", "objects/sun/");
	ground_extension = gps::Model3D("objects/ground/ground_extension.obj", "objects/ground/");
}

void initShaders()
{
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	depthMapShader.loadShader("shaders/simpleDepthMap.vert", "shaders/simpleDepthMap.frag");
}

void initUniforms()
{
	myCustomShader.useShaderProgram();

	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");

	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");

	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");

	lightDirMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDirMatrix");

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	// lightDir = glm::vec3(0.0f, 500.0f, 200.0f);
	lightDir = glm::vec3(0.0f, 0.0f, 0.0f);
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void initSkybox()
{
	std::vector<const GLchar*> faces;
	faces.push_back("textures/sor_beach/beach_rt.JPG");
	faces.push_back("textures/sor_beach/beach_lf.JPG");
	faces.push_back("textures/sor_beach/beach_up.JPG");
	faces.push_back("textures/sor_beach/beach_dn.JPG");
	faces.push_back("textures/sor_beach/beach_bk.JPG");
	faces.push_back("textures/sor_beach/beach_ft.JPG");

	mySkyBox.Load(faces);
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();

	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
	glm::value_ptr(view));
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
	glm::value_ptr(projection));
}

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	processMovement();

	//render the scene to the depth buffer (first pass)

	depthMapShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	//create model matrix for nanosuit
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));

	myModel.Draw(depthMapShader);

	//create model matrix for ground
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
						1,
						GL_FALSE,
						glm::value_ptr(model));

	ground.Draw(depthMapShader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//render the scene (second pass)

	myCustomShader.useShaderProgram();
    glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "fogDensity"), fogDensity);

	//send lightSpace matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	//send view matrix to shader
	view = myCamera.getViewMatrix();


	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "view"),
		1,
		GL_FALSE,
		glm::value_ptr(view));

	//compute light direction transformation matrix
	lightDirMatrix = glm::mat3(glm::inverseTranspose(view));
	//send lightDir matrix data to shader
	glUniformMatrix3fv(lightDirMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightDirMatrix));

	glViewport(0, 0, retina_width, retina_height);
    glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "diffuseStrength"), 1.0f);
    glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "specularStrength"), 0.5f);
    glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "shininess"), 64.0f);
	myCustomShader.useShaderProgram();

	//bind the depth map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

	//create model matrix for nanosuit
	model = glm::mat4(1.0f);
	//send model matrix data to shader
	// glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	myModel.Draw(myCustomShader);

	// model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -50.0f, 0.0f));
	// glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	model = glm::mat4(1.0f);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	island.Draw(myCustomShader);

	//create model matrix for ground
	// model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -250.0f, 0.0f));
	//send model matrix data to shader
	// glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//create normal matrix
    model = glm::translate(glm::mat4(1.0f), glm::vec3(groundDeltaX, groundDeltaY, 0.0f));
    // model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "diffuseStrength"), 3.0f);
    glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "specularStrength"), 0.8f);
    glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "shininess"), 128.0f);

    depthMapShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix2()));
    // model = glm::translate(glm::mat4(1.0f), glm::vec3(-206.0f, -135.0f, 6.5f));
    // glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
    // 	1,
    // 	GL_FALSE,
    // 	glm::value_ptr(model));
    //
    // // docks.Draw(depthMapShader);
    // barrel.Draw(depthMapShader);
    // lantern.Draw(depthMapShader);
    // bottle.Draw(depthMapShader);

    myCustomShader.useShaderProgram();

    docks.Draw(myCustomShader);

    lantern.Draw(myCustomShader);
    bottle.Draw(myCustomShader);

    if(drawWireframe == true)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

	depthMapShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix2()));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(glm::vec3(0.0f)));


	barrel.Draw(depthMapShader);
	princess.Draw(depthMapShader);
	bottle.Draw(depthMapShader);
	docks.Draw(depthMapShader);


	myCustomShader.useShaderProgram();
    barrel.Draw(myCustomShader);
    princess.Draw(myCustomShader);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(-530.0f, 279.0f, 0.0f));
    model = glm::rotate(model, glm::radians(diamondRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(530.0f, -279.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    diamond.Draw(myCustomShader);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    diamondRotation += 1.0f;

    groundDeltaX += 0.01f * groundDirectionX;
    if(groundDeltaX <= groundMaxDeltaLeft || groundDeltaX >= groundMaxDeltaRight)
    {
        groundDirectionX *= -1;
    }

    groundDeltaY += 0.03f * groundDirectionY;
    if(groundDeltaY <= groundMaxDeltaDown || groundDeltaY >= groundMaxDeltaUp)
    {
        groundDirectionY *= -1;
    }
    // std::cout << groundDelta << ' ' << groundMaxDelta << ' ' << glm::abs(groundDelta) - groundMaxDelta << '\n';

	lightAngle += 0.1f;
	if (lightAngle > 360.0f)
		lightAngle -= 360.0f;
	glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
	myCustomShader.useShaderProgram();
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));

	//draw a white cube around the light

	lightShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

	model = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, lightDir);
	model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// lightCube.Draw(lightShader);


    lightAngle2 += 0.5f;
	if (lightAngle2 > 360.0f)
		lightAngle2 -= 360.0f;
	lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle2), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir2, 1.0f));
	myCustomShader.useShaderProgram();
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	//send lightSpace matrix to shader
	// glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
	// 	1,
	// 	GL_FALSE,
	// 	glm::value_ptr(computeLightSpaceTrMatrix2()));

    lightShader.useShaderProgram();


    model = glm::translate(glm::mat4(1.0f), lightDir2);
    model = glm::rotate(model, glm::radians(lightAngle2), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, -lightDir2);
    model = glm::translate(model, lightDir2);



    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	// lightCube2.Draw(lightShader);

	mySkyBox.Draw(skyboxShader, view, projection);

    if(rain.is_raining())
    {
        rain.Draw(myCustomShader);
        rain.applyWeight();
        // printf("Rain drops: %u\n", rain.getNoDrops());
    }

	myCustomShader.useShaderProgram();

	// tanslate the helicopter and rotate the blades
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, heli_base_offset));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	helicopter_base.Draw(myCustomShader);

	model = glm::translate(glm::mat4(1.0f), heli_blades1_origpos + glm::vec3(0.0f, 0.0f, heli_base_offset));
	model = glm::rotate(model, glm::radians(heli_blades_angle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, -(heli_blades1_origpos + glm::vec3(0.0f, 0.0f, heli_base_offset)));
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, heli_base_offset));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	helicopter_blades1.Draw(myCustomShader);

	model = glm::translate(glm::mat4(1.0f), heli_blades2_origpos + glm::vec3(0.0f, 0.0f, heli_base_offset));
	model = glm::rotate(model, glm::radians(-heli_blades_angle), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, -(heli_blades2_origpos + glm::vec3(0.0f, 0.0f, heli_base_offset)));
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, heli_base_offset));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	helicopter_blades2.Draw(myCustomShader);

	// move the helicopter base
	// model =

	heli_base_offset -= 1.0f;
	heli_blades_angle += 3.0f;
	// printf("%.2f\n", heli_base_offset);
	if(heli_base_offset < -2500.0f)
	{
		heli_base_offset = 500.0f;
	}

	if(mouseAnimationEnabled)
	{
		GLdouble xpos, ypos;
		glfwGetCursorPos(glWindow, &xpos, &ypos);
	    delta_x = (GLfloat)xpos - start_x + autoMouseAngle;
	    delta_y = (GLfloat)ypos - start_y;
	    // std :: cout << delta_x << ' ' << delta_y << '\n';

	    float sensitivity = 0.05f;
	    delta_x *= sensitivity;
	    delta_y *= sensitivity;

	    if (delta_y > 89.0f){
	        delta_y = 89.0f;
	    }
	    if (delta_y < -89.0f){
	        delta_y = -89.0f;
	    }

	    myCamera.rotate(-delta_y, delta_x);
		myCamera.move(gps::MOVE_FORWARD, 0.5f);

		autoMouseAngle += 1.5f * autoMouseDirection;
		if(std::abs(autoMouseAngle) > 60.0f)
		{
			autoMouseDirection *= -1;
		}
	}


	model = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(0.0f, 1000.0f, 0.0f));
	model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	sun.Draw(myCustomShader);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-190.0f, 50.0f, -178.0f));
	model = glm::scale(model, glm::vec3(0.002f, 1.0f, 0.002f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	// ground_extension.Draw(myCustomShader);

    // transparent objects should be rendered after the opaque ones
    myCustomShader.useShaderProgram();
    glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "alphaFactor"), alphaFactor);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(groundDeltaX, groundDeltaY, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ground.Draw(myCustomShader);
    glDisable(GL_BLEND);

}

int main(int argc, const char * argv[]) {

	initOpenGLWindow();
	initOpenGLState();
	initFBOs();
	initModels();
	initShaders();
	initUniforms();
	initSkybox();
	glCheckError();
	//glfwSetCursorPos(glWindow, glWindowWidth / 2, glWindowHeight / 2);
    std::srand(std::time(0));
	myCamera.rotate(0.0f, 0.0f);
	act_time = last_time = glfwGetTime();
	monitor = glfwGetPrimaryMonitor();
	vidMode = glfwGetVideoMode(monitor);

	while (!glfwWindowShouldClose(glWindow)) {
		renderScene();



		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}
    rain.clear();

	//close GL context and any other GLFW resources
	glfwTerminate();

	return 0;
}
