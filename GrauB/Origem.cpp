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

// Dimens�es da janela
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Vari�veis de controle de transla��o
bool translateX = false, translateY = false, translateZ = false;
int translateDirection = 0;

// Vari�vel de controle de escala
float scale = 0.0;

// ID do objeto selecionado
SceneObj* selectedObject = nullptr;
Scene* gScene = nullptr;

// Reseta vari�veis de controle de escala
void resetScaleVariable() {
	scale = 0.0;
}

// Reseta vari�veis de controle de transla��o
void resetTranslationVariables() {
	translateX = false;
	translateY = false;
	translateZ = false;
	translateDirection = 0;
}

// Ajusta a escala com base na tecla pressionada.
void adjustScale(int key)
{
	float scaleFactor = 0.05;

	if (key == GLFW_KEY_KP_ADD)
		scale += scaleFactor;
	else if (key == GLFW_KEY_KP_SUBTRACT)
		scale -= scaleFactor;
}

// Ajusta a rota��o com base na tecla pressionada.
void adjustRotation(int key)
{
	if (selectedObject != nullptr) {
		switch (key)
		{
		case(GLFW_KEY_X):
			selectedObject->rotate = "x";
			break;
		case(GLFW_KEY_Y):
			selectedObject->rotate = "y";
			break;
		case(GLFW_KEY_Z):
			selectedObject->rotate = "z";
			break;
		default:
			break;
		}
	}
}

void adjustPlayCurve(int key) {
	if (key == GLFW_KEY_P && selectedObject != nullptr)
		selectedObject->playCurve = !selectedObject->playCurve;
}

// Ajusta a transla��o com base na tecla pressionada.
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
	if (selectedObject != nullptr)
		selectedObject->rotate = "";

	if (id < 0)
		selectedObject = nullptr;
	else {
		for (int i = 0; i < gScene->sceneObject.size(); ++i)
			if (gScene->sceneObject[i].transfObjectId == id) {
				selectedObject = &gScene->sceneObject[i];
				break;
			}
	}
	resetTranslationVariables();
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

// Fun��o callback acionada quando h� intera��o com o teclado
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS) {
		adjustScale(key);
		adjustRotation(key);
		adjustPlayCurve(key);
		selectObjectByKey(key);
	}

	adjustTranslation(key);

	if (gScene != nullptr)
		gScene->camera.moveCamera(key);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (gScene != nullptr)
		gScene->camera.updateCameraDirection(xpos, ypos);
}

void scrollCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (gScene != nullptr)
		gScene->camera.scrollCamera(ypos);
}

int main()
{
	// Inicializa��o da GLFW
	glfwInit();

	// Cria��o da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Grau B - Gustavo, Arthur e Rafael", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da fun��o de callback para a janela GLFW
	glfwSetKeyCallback(window, keyCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);

	// GLAD: carrega todos os ponteiros d fun��es da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Falha ao inicializar o GLAD" << std::endl;
	}

	// Obtendo as informa��es de vers�o
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderizador: " << renderer << endl;
	cout << "Vers�o OpenGL suportada" << version << endl;

	// Definindo as dimens�es da viewport com as mesmas dimens�es da janela da aplica��o
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Compilando e buildando o programa de shader
	Shader shader("VShader.vs", "FShader.fs");
	glUseProgram(shader.ID);

	Scene scene = Scene("Scene.json", &shader, width, height);
	gScene = &scene;

	// Ilumina��o: Define a posi��o da fonte de luz
	shader.setVec3("light_pos", scene.lightPositionX, scene.lightPositionY, scene.lightPositionZ);
	// Ilumina��o: Define a cor da luz
	shader.setVec3("light_color", scene.lightColorR, scene.lightColorG, scene.lightColorB);


	glEnable(GL_DEPTH_TEST);


	// Loop da aplica��o
	while (!glfwWindowShouldClose(window))
	{
		resetTranslationVariables();

		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as fun��es de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(0.1f, 0.1f, 0.1f, 0.1f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		if (gScene != nullptr)
			gScene->camera.updateCamera();

		for (int i = 0; i < scene.sceneObject.size(); ++i)
		{
			// Ilumina��o: Coeficiente de material para a luz ambiente
			shader.setFloat("ka", scene.sceneObject[i].sceneObjInfo.ka);
			// Ilumina��o: Coeficiente de material para a luz difusa
			shader.setFloat("kd", scene.sceneObject[i].sceneObjInfo.kd);
			// Ilumina��o: Coeficiente de material para a luz especular
			shader.setFloat("ks", scene.sceneObject[i].sceneObjInfo.ks);
			// Ilumina��o: Expoente de brilho do material
			shader.setFloat("q", scene.sceneObject[i].sceneObjInfo.ns);

			if (scene.sceneObject[i].rotate == "x")
				scene.sceneObject[i].rotateX();
			else if (scene.sceneObject[i].rotate == "y")
				scene.sceneObject[i].rotateY();
			else if (scene.sceneObject[i].rotate == "z")
				scene.sceneObject[i].rotateZ();

			if (selectedObject != nullptr && selectedObject->transfObjectId >= 0 && selectedObject->transfObjectId == scene.sceneObject[i].transfObjectId) {

				if (!scene.sceneObject[i].playCurve || scene.sceneObject[i].nbCurve <= 0) {
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

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	for (int i = 0; i < scene.sceneObject.size(); ++i) {
		glDeleteVertexArrays(1, &scene.sceneObject[i].sceneObjInfo.VAO);
	}

	// Finaliza a execu��o da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}