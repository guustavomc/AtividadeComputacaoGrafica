
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

#include "stb_image.h"
#include "Shader.h"
#include "SceneObject.cpp"

// Dimensões da janela
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Variáveis de controle de rotação
bool rotateX = false, rotateY = false, rotateZ = false;

// Variáveis de controle de translação
bool translateX = false, translateY = false, translateZ = false;
int translateDirection = 0;

// Variável de controle de escala
float scale = 1.0;

// Ajusta a escala com base na tecla pressionada.
void adjustScale(int key)
{
	float scaleFactor = 0.05;

	if (key == GLFW_KEY_KP_ADD)
		scale += scale * scaleFactor;
	else if (key == GLFW_KEY_KP_SUBTRACT)
		scale -= scale * scaleFactor;
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
	case(GLFW_KEY_W):
		translateX = false;
		translateY = false;
		translateZ = true;
		translateDirection = 1;
		break;
	case(GLFW_KEY_S):
		translateX = false;
		translateY = false;
		translateZ = true;
		translateDirection = -1;
		break;
	default:
		break;
	}
}

// Função callback acionada quando há interação com o teclado
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	adjustScale(key);
	adjustRotation(key);
	adjustTranslation(key);
}

// Reseta variáveis de controle de translação
void resetTranslationVariables() {
	translateX = false;
	translateY = false;
	translateZ = false;
	translateDirection = 0;
}

// Cria e retorna um vetor de objetos da cena, representando cubos, distribuídos horizontalmente, com base no número fornecido (numObjects)
std::vector<SceneObject> generateSceneObjects(int numObjects, GLuint vertexArrayObject, int numVertices, Shader* shader, GLuint textureId) {
	std::vector<SceneObject> objects;

	const float horizontalSpacing = 2.75f;

	for (int i = 0; i < numObjects; ++i)
	{
		float xPosition = 0.0f;

		if (i % 2 == 0)
			xPosition = (-horizontalSpacing) * (i / 2);
		else
			xPosition = (horizontalSpacing) * ((i / 2) + 1);

		objects.push_back(SceneObject(vertexArrayObject, numVertices, shader, textureId, glm::vec3(xPosition, 0.0, 0.0)));
	}

	return objects;
}

// Função para ler o arquivo OBJ e extrair os dados de vértices e índices
bool readOBJFile(const std::string& filepath, std::vector<glm::vec3>& vertices, std::vector<GLuint>& indices, std::vector<GLfloat>& vbuffer,
	std::vector<glm::vec2>& textureCoordinates, string& materialFileName, string& materialName) {

	// Abrindo o arquivo OBJ
	std::ifstream inputFile(filepath);
	if (!inputFile.is_open()) {
		std::cerr << "Erro ao abrir o arquivo OBJ: " << filepath << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(inputFile, line)) {
		std::istringstream ssline(line);
		std::string word;
		ssline >> word;

		if (word == "mtllib") {
			ssline >> materialFileName;
		}
		else if (word == "usemtl") {
			ssline >> materialName;
		}
		else if (word == "v") {
			glm::vec3 v;
			ssline >> v.x >> v.y >> v.z;
			vertices.push_back(v);
		}
		else if (word == "vt")
		{
			glm::vec2 vt;
			ssline >> vt.s >> vt.t;
			textureCoordinates.push_back(vt);
		}
		else if (word == "f") {
			std::string tokens[3];
			ssline >> tokens[0] >> tokens[1] >> tokens[2];

			for (int i = 0; i < 3; ++i) {
				int posLastValue = tokens[i].find_last_of('/');
				std::string lastValue = tokens[i].substr(posLastValue + 1);
				int normal = std::stoi(lastValue);

				int pos = tokens[i].find("/");
				std::string token = tokens[i].substr(0, pos);
				int index = std::atoi(token.c_str()) - 1;
				indices.push_back(index);

				vbuffer.push_back(vertices[index].x);
				vbuffer.push_back(vertices[index].y);
				vbuffer.push_back(vertices[index].z);

				vbuffer.push_back(vertices[index].r);
				vbuffer.push_back(vertices[index].g);
				vbuffer.push_back(vertices[index].b);

				// Movendo para a próxima parte da string para obter o índice da textura
				tokens[i] = tokens[i].substr(pos + 1);
				pos = tokens[i].find("/");
				token = tokens[i].substr(0, pos);
				index = atoi(token.c_str()) - 1; // Convertendo o índice para inteiro e ajustando para começar de 0

				// Adicionando as coordenadas da textura ao buffer de vértices
				vbuffer.push_back(textureCoordinates[index].s);
				vbuffer.push_back(textureCoordinates[index].t);
			}
		}
	}

	inputFile.close();
	return true;
}

// Função para inicializar os buffers de vértices e arrays de vértices (VAO e VBO)
bool initializeBuffers(GLuint& VBO, GLuint& VAO, const std::vector<GLfloat>& vbuffer, int stride) {
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vbuffer.size() * sizeof(GLfloat), vbuffer.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Especificando os atributos do vértice
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

// Função principal para carregar um arquivo OBJ e inicializar os buffers de vértices e arrays de vértices (VAO e VBO)
int loadSimpleOBJ(const std::string& filepath, int& numVertices, string& materialFileName, string& materialName) {
	std::vector<glm::vec3> vertices;
	std::vector<GLuint> indices;
	vector <glm::vec2> textureCoordinates;
	std::vector<GLfloat> vbuffer;
	int stride = 8;

	if (!readOBJFile(filepath, vertices, indices, vbuffer, textureCoordinates, materialFileName, materialName)) {
		std::cerr << "Erro ao ler o arquivo OBJ: " << filepath << std::endl;
		return -1;
	}

	numVertices = vbuffer.size() / stride;

	GLuint VBO, VAO;
	if (!initializeBuffers(VBO, VAO, vbuffer, stride)) {
		std::cerr << "Erro ao inicializar os buffers de vértices e arrays de vértices." << std::endl;
		return -1;
	}

	return VAO;
}

// Carrega uma textura a partir de um arquivo, configura seus parâmetros e retorna o ID da textura
int loadTexture(string filepath)
{
	GLuint texID;

	// Gera o identificador da textura na memória 
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	//Ajusta os parâmetros de wrapping e filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Carregamento da imagem
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		GLenum format;
		switch (nrChannels) {
		case 1:
			format = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default:
			std::cerr << "Número de canais não suportado: " << nrChannels << std::endl;
			stbi_image_free(data);
			return 0;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Falha ao carregar a textura" << std::endl;
		return 0;
	}

	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

// Carrega um arquivo MTL simples, extrai o nome do arquivo de textura e o armazena na variável textureFileName
string loadSimpleMTL(const std::string& filepath, string materialName)
{
	string textureFileName;
	std::ifstream inputFile(filepath);

	if (!inputFile.is_open()) {
		std::cerr << "Erro ao abrir o arquivo MTL: " << filepath << std::endl;
		return "";
	}

	string line;
	bool materialFound = false;

	while (getline(inputFile, line))
	{
		istringstream ssline(line);
		string word;
		ssline >> word;

		if (word == "newmtl")
		{
			string currentMaterialName;
			ssline >> currentMaterialName;
			materialFound = (currentMaterialName == materialName);
		}
		else if (word == "map_Kd" && materialFound) {
			ssline >> textureFileName;
			break;
		}
	}

	inputFile.close();
	return textureFileName;
}

int main()
{
	// Inicialização da GLFW
	glfwInit();

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola 3D", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, keyCallback);

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

	//Matriz de view
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	shader.setMat4("view", value_ptr(view));

	//Matriz de projeção perspectiva - definindo o volume de visualização (frustum)
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	shader.setMat4("projection", glm::value_ptr(projection));

	glEnable(GL_DEPTH_TEST);

	int numVertices;
	string materialFileName;
	string materialName;
	GLuint VAO = loadSimpleOBJ("../3D_Models/Suzanne/SuzanneTriTextured.obj", numVertices, materialFileName, materialName);
	//GLuint VAO = loadSimpleOBJ("../3D_models/Cube/cube.obj", numVertices, materialFileName, materialName);

	// Carregamento do arquivo MTL para obter as informações do material
	string textureFileName = loadSimpleMTL(materialFileName, materialName);

	//Carregando uma textura e armazenando o identificador na memória
	GLuint textureId = loadTexture(textureFileName);

	int numObjetcts = 7;
	std::vector<SceneObject> sceneObjects = generateSceneObjects(numObjetcts, VAO, numVertices, &shader, textureId);

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

		for (int i = 0; i < sceneObjects.size(); ++i)
		{
			if (rotateX)
				sceneObjects[i].rotateX();
			else if (rotateY)
				sceneObjects[i].rotateY();
			else if (rotateZ)
				sceneObjects[i].rotateZ();

			if (translateX)
				sceneObjects[i].translateX(translateDirection);
			else if (translateY)
				sceneObjects[i].translateY(translateDirection);
			else if (translateZ)
				sceneObjects[i].translateZ(translateDirection);

			sceneObjects[i].setScale(glm::vec3(scale, scale, scale));
			sceneObjects[i].updateModelMatrix();
			sceneObjects[i].renderObject();
		}

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}
