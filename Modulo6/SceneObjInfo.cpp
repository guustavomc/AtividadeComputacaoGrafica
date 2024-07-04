#pragma once
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Common/include/stb_image.h"

using namespace std;

class SceneObjInfo {
public:
	int numVertices;
	float ka, kd, ks, ns;
	GLuint VAO, textureId;

	SceneObjInfo(string objFilePath) : objFilePath(objFilePath)
	{
		this->VAO = loadSimpleOBJ(objFilePath, numVertices, materialFileName, materialName);
		this->textureFileName = loadSimpleMTL(materialFileName, materialName);
		this->textureId = loadTexture(textureFileName);
	}

private:
	string objFilePath, materialFileName, materialName, textureFileName;

	bool readOBJFile(const std::string& filepath, std::vector<GLuint>& indices, std::vector<GLfloat>& vbuffer,
		string& materialFileName, string& materialName) {

		glm::vec3 color = glm::vec3(1.0, 0.0, 1.0);

		vector <glm::vec2> textureCoordinates;
		vector <glm::vec3> vertices;
		vector <glm::vec3> normals;

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
				std::getline(ssline >> std::ws, materialFileName);
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
			else if (word == "vn")
			{
				glm::vec3 vn;
				ssline >> vn.x >> vn.y >> vn.z;
				normals.push_back(vn);
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

					vbuffer.push_back(color.r);
					vbuffer.push_back(color.g);
					vbuffer.push_back(color.b);

					tokens[i] = tokens[i].substr(pos + 1);
					pos = tokens[i].find("/");
					token = tokens[i].substr(0, pos);
					index = atoi(token.c_str()) - 1;

					vbuffer.push_back(textureCoordinates[index].s);
					vbuffer.push_back(textureCoordinates[index].t);

					tokens[i] = tokens[i].substr(pos + 1);
					index = atoi(tokens[i].c_str()) - 1;

					vbuffer.push_back(normals[index].x);
					vbuffer.push_back(normals[index].y);
					vbuffer.push_back(normals[index].z);
				}
			}
		}

		inputFile.close();
		return true;
	}

	bool initializeBuffers(GLuint& VBO, GLuint& VAO, const std::vector<GLfloat>& vbuffer, int stride) {
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vbuffer.size() * sizeof(GLfloat), vbuffer.data(), GL_STATIC_DRAW);

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
		glEnableVertexAttribArray(3);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		return true;
	}

	int loadSimpleOBJ(const std::string& filepath, int& numVertices, string& materialFileName, string& materialName) {
		std::vector<GLuint> indices;
		std::vector<GLfloat> vbuffer;
		int stride = 11;

		if (!readOBJFile(filepath, indices, vbuffer, materialFileName, materialName)) {
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
			else if (word == "Ka" && materialFound) {
				ssline >> ka;
				ssline.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}
			else if (word == "Ks" && materialFound) {
				ssline >> ks;
				ssline.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}
			else if (word == "Kd" && materialFound) {
				ssline >> kd;
				ssline.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}
			else if (word == "Ns" && materialFound) {
				ssline >> ns;
				ssline.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}
			else if (word == "map_Kd" && materialFound) {
				ssline >> textureFileName;
				break;
			}
		}

		inputFile.close();
		return textureFileName;
	}

	int loadTexture(string filepath)
	{
		GLuint texID;

		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

};