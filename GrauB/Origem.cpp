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

// Dimensões da janela
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Variáveis de controle de rotação
bool rotateX = false, rotateY = false, rotateZ = false;

// Variáveis de controle de translação
bool translateX = false, translateY = false, translateZ = false;
int translateDirection = 0;

// Variável de controle de escala
float scale = 0.0;

Camera* gCamera = nullptr;

// ID do objeto selecionado
int selectedObjectId = -1;

//// Controle da curva
//bool playCurve = true;

// Reseta variáveis de controle de escala
void resetScaleVariable() {
	scale = 0.0;
}

// Reseta variáveis de controle de translação
void resetTranslationVariables() {
	translateX = false;
	translateY = false;
	translateZ = false;
	translateDirection = 0;
}

// Reseta variáveis de controle de rotação
void resetRotationVariables() {
	rotateX = false;
	rotateY = false;
	rotateZ = false;
}

//void resetPlayCurve() {
//	playCurve = true;
//}

// Ajusta a escala com base na tecla pressionada.
void adjustScale(int key)
{
	float scaleFactor = 0.05;

	if (key == GLFW_KEY_KP_ADD)
		scale += scaleFactor;
	else if (key == GLFW_KEY_KP_SUBTRACT)
		scale -= scaleFactor;
}

// Ajusta a rotação com base na tecla pressionada.
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

//void adjustPlayCurve(int key) {
//	if (key == GLFW_KEY_P)
//		playCurve = !playCurve;
//}

// Ajusta a translação com base na tecla pressionada.
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
	//resetPlayCurve();
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

// Função callback acionada quando há interação com o teclado
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
	// Inicialização da GLFW
	glfwInit();

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Modulo 6", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, keyCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Falha ao inicializar o GLAD" << std::endl;
	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderizador: " << renderer << endl;
	cout << "Versão OpenGL suportada" << version << endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Compilando e buildando o programa de shader
	Shader shader("VShader.vs", "FShader.fs");
	glUseProgram(shader.ID);

	Scene scene = Scene("Scene.json", &shader, width, height);
	gCamera = &scene.camera;

	// Iluminação: Define a posição da fonte de luz
	shader.setVec3("light_pos", scene.lightPositionX, scene.lightPositionY, scene.lightPositionZ);
	// Iluminação: Define a cor da luz
	shader.setVec3("light_color", scene.lightColorR, scene.lightColorG, scene.lightColorB);


	glEnable(GL_DEPTH_TEST);


	// Loop da aplicação
	while (!glfwWindowShouldClose(window))
	{
		resetTranslationVariables();

		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
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

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	for (int i = 0; i < scene.sceneObject.size(); ++i) {
		glDeleteVertexArrays(1, &scene.sceneObject[i].sceneObjInfo.VAO);
	}

	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}