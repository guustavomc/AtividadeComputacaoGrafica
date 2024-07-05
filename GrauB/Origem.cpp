#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Common/include/stb_image.h"
#include "../Common/include/Shader.h"
#include "Camera.cpp"
#include "Scene.cpp"
#include "SceneObj.cpp"
#include "Bezier.cpp"

const GLuint WIDTH = 1000, HEIGHT = 1000;
bool rotateX = false, rotateY = false, rotateZ = false;
bool translateX = false, translateY = false, translateZ = false;
int translateDirection = 0;
float scale = 0.0;

Camera* gCamera = nullptr;
int selectedObjectId = -1;

void resetScaleVariable() {
	scale = 0.0;
}

void resetTranslationVariables() {
	translateX = false;
	translateY = false;
	translateZ = false;
	translateDirection = 0;
}

void resetRotationVariables() {
	rotateX = false;
	rotateY = false;
	rotateZ = false;
}


void adjustScale(int key)
{
	float scaleFactor = 0.05;

	if (key == GLFW_KEY_KP_ADD)
		scale += scaleFactor;
	else if (key == GLFW_KEY_KP_SUBTRACT)
		scale -= scaleFactor;
}

void adjustRotation(int key)
{
	switch (key)
	{
	case(GLFW_KEY_X):
		rotateX = true;
		rotateY = false;
		rotateZ = false;
		break;
	case(GLFW_KEY_Y):
		rotateX = false;
		rotateY = true;
		rotateZ = false;
		break;
	case(GLFW_KEY_Z):
		rotateX = false;
		rotateY = false;
		rotateZ = true;
		break;
	default:
		break;
	}

}

void adjustTranslation(int key)
{
	switch (key)
	{
	case(GLFW_KEY_RIGHT):
		translateX = true;
		translateY = false;
		translateZ = false;
		translateDirection = 1;
		break;
	case(GLFW_KEY_LEFT):
		translateX = true;
		translateY = false;
		translateZ = false;
		translateDirection = -1;
		break;
	case(GLFW_KEY_UP):
		translateX = false;
		translateY = true;
		translateZ = false;
		translateDirection = 1;
		break;
	case(GLFW_KEY_DOWN):
		translateX = false;
		translateY = true;
		translateZ = false;
		translateDirection = -1;
		break;
	case(GLFW_KEY_I):
		translateX = false;
		translateY = false;
		translateZ = true;
		translateDirection = 1;
		break;
	case(GLFW_KEY_K):
		translateX = false;
		translateY = false;
		translateZ = true;
		translateDirection = -1;
		break;
	default:
		break;
	}
}

void setSelectedObject(int id) {
	selectedObjectId = id;
	resetTranslationVariables();
	resetRotationVariables();
	resetScaleVariable();
}

void selectObjectByKey(int key) {
	switch (key) {
	case GLFW_KEY_KP_0: case GLFW_KEY_0: setSelectedObject(0); break;
	case GLFW_KEY_KP_1: case GLFW_KEY_1: setSelectedObject(1); break;
	case GLFW_KEY_KP_2: case GLFW_KEY_2: setSelectedObject(2); break;
	case GLFW_KEY_KP_3: case GLFW_KEY_3: setSelectedObject(3); break;
	case GLFW_KEY_KP_4: case GLFW_KEY_4: setSelectedObject(4); break;
	case GLFW_KEY_KP_5: case GLFW_KEY_5: setSelectedObject(5); break;
	case GLFW_KEY_KP_6: case GLFW_KEY_6: setSelectedObject(6); break;
	case GLFW_KEY_KP_7: case GLFW_KEY_7: setSelectedObject(7); break;
	case GLFW_KEY_KP_8: case GLFW_KEY_8: setSelectedObject(8); break;
	case GLFW_KEY_KP_9: case GLFW_KEY_9: setSelectedObject(9); break;
	case GLFW_KEY_KP_ENTER: case GLFW_KEY_ENTER: setSelectedObject(-1); break;
	default: break;
	}
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS) {
		adjustScale(key);
		adjustRotation(key);
		adjustTranslation(key);
		//adjustPlayCurve(key);
		selectObjectByKey(key);
	}

	if (gCamera)
		gCamera->moveCamera(key);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (gCamera)
		gCamera->updateCameraDirection(xpos, ypos);
}

void scrollCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (gCamera)
		gCamera->scrollCamera(ypos);
}

int main()
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Modulo 6", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, keyCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Falha ao inicializar o GLAD" << std::endl;
	}

	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderizador: " << renderer << endl;
	cout << "Versão OpenGL suportada" << version << endl;

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	Shader shader("VShader.vs", "FShader.fs");
	glUseProgram(shader.ID);

	Scene scene = Scene("Scene.json", &shader, width, height);
	gCamera = &scene.camera;

	shader.setVec3("light_pos", scene.lightPositionX, scene.lightPositionY, scene.lightPositionZ);
	shader.setVec3("light_color", scene.lightColorR, scene.lightColorG, scene.lightColorB);


	glEnable(GL_DEPTH_TEST);


	while (!glfwWindowShouldClose(window))
	{
		resetTranslationVariables();

		glfwPollEvents();

		glClearColor(0.188f, 0.666f, 0.9333f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		gCamera->updateCamera();

		for (int i = 0; i < scene.sceneObject.size(); ++i)
		{
			// Iluminação: Coeficiente de material para a luz ambiente
			shader.setFloat("ka", scene.sceneObject[i].sceneObjInfo.ka);
			// Iluminação: Coeficiente de material para a luz difusa
			shader.setFloat("kd", scene.sceneObject[i].sceneObjInfo.kd);
			// Iluminação: Coeficiente de material para a luz especular
			shader.setFloat("ks", scene.sceneObject[i].sceneObjInfo.ks);
			// Iluminação: Expoente de brilho do material
			shader.setFloat("q", scene.sceneObject[i].sceneObjInfo.ns);

			if (selectedObjectId >= 0 && selectedObjectId == scene.sceneObject[i].objectId) {
				//scene.sceneObject[i].playCurve = playCurve;

				if (rotateX)
					scene.sceneObject[i].rotateX();
				else if (rotateY)
					scene.sceneObject[i].rotateY();
				else if (rotateZ)
					scene.sceneObject[i].rotateZ();

				if (!scene.sceneObject[i].playCurve) {
					if (translateX)
						scene.sceneObject[i].translateX(translateDirection);
					else if (translateY)
						scene.sceneObject[i].translateY(translateDirection);
					else if (translateZ)
						scene.sceneObject[i].translateZ(translateDirection);
				}

				scene.sceneObject[i].updateScale(scale);
			}

			if (scene.sceneObject[i].playCurve && scene.sceneObject[i].nbCurve > 0) {
				glm::vec3 curvePosition = scene.sceneObject[i].curveBezier.getPointOnCurve(scene.sceneObject[i].iPoint);
				scene.sceneObject[i].updatePosition(curvePosition);
				scene.sceneObject[i].iPoint = (scene.sceneObject[i].iPoint + 1) % scene.sceneObject[i].nbCurve;
			}

			scene.sceneObject[i].updateModelMatrix();
			scene.sceneObject[i].renderObject();
		}

		resetScaleVariable();

		glfwSwapBuffers(window);
	}
	for (int i = 0; i < scene.sceneObject.size(); ++i) {
		glDeleteVertexArrays(1, &scene.sceneObject[i].sceneObjInfo.VAO);
	}

	glfwTerminate();
	return 0;
}