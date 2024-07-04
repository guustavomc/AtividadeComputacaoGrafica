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
#include "SceneObj.cpp"
#include "Camera.cpp"

using namespace std;
using json = nlohmann::json;

struct SceneObjAux {
	int objectId;
	float x, y, z;
	string objFilePath;
	vector <glm::vec3> curvePoints;
};

struct SceneCameraAux {
	float fov, nearPlane, farPlane, positionX, positionY, positionZ,
		frontDirectionX, frontDirectionY, frontDirectionZ,
		upDirectionX, upDirectionY, upDirectionZ;
};

class Scene
{
public:
	Camera camera;
	vector<SceneObj> sceneObject;
	int width, height;
	float lightPositionX, lightPositionY, lightPositionZ, lightColorR, lightColorG, lightColorB;

	Scene(string jsonFilePath, Shader* shader, int width, int height)
		: jsonFilePath(jsonFilePath), shader(shader), width(width), height(height), camera(shader, width, height)
	{
		loadSceneFromJSON(jsonFilePath);
		loadObjects();
	}

private:
	Shader* shader;
	string jsonFilePath;
	std::vector<SceneObjAux> sceneObjectsAux;

	void loadSceneFromJSON(const std::string& jsonFilePath) {
		std::ifstream file(jsonFilePath);
		if (!file.is_open()) {
			std::cerr << "Não foi possível abrir o arquivo JSON: " << jsonFilePath << std::endl;
			return;
		}

		json j;
		file >> j;

		if (j.contains("objects")) {
			for (const auto& obj : j["objects"]) {
				SceneObjAux objAux;
				objAux.objectId = obj["objectId"];
				objAux.objFilePath = obj["objFilePath"];
				objAux.x = obj["positionX"];
				objAux.y = obj["positionY"];
				objAux.z = obj["positionZ"];
				if (obj.contains("curvePoints")) {
					for (const auto& point : obj["curvePoints"]) {
						float x = std::stof(point[0].get<std::string>());
						float y = std::stof(point[1].get<std::string>());
						float z = std::stof(point[2].get<std::string>());
						objAux.curvePoints.push_back(glm::vec3(x, y, z));
					}
				}

				sceneObjectsAux.push_back(objAux);
			}
		}
		else {
			std::cerr << "Estrutura JSON inválida: 'objects' não encontrado." << std::endl;
		}

		loadLightFromJSON(j);
		loadCameraFromJSON(j);
	}

	void loadLightFromJSON(json j) {
		if (j.contains("light")) {
			const auto& light = j["light"];
			if (light.contains("lightPositionX")) {
				lightPositionX = light["lightPositionX"];
			}
			if (light.contains("lightPositionY")) {
				lightPositionY = light["lightPositionY"];
			}
			if (light.contains("lightPositionZ")) {
				lightPositionZ = light["lightPositionZ"];
			}
			if (light.contains("lightColorR")) {
				lightColorR = light["lightColorR"];
			}
			if (light.contains("lightColorG")) {
				lightColorG = light["lightColorG"];
			}
			if (light.contains("lightColorB")) {
				lightColorB = light["lightColorB"];
			}
		}
		else {
			std::cerr << "Estrutura JSON inválida: 'lights' não encontrado." << std::endl;
		}
	}

	void loadCameraFromJSON(json j) {
		if (j.contains("camera")) {
			const auto& cam = j["camera"];
			SceneCameraAux cameraAux;

			if (cam.contains("fov")) {
				cameraAux.fov = cam["fov"];
			}
			if (cam.contains("nearPlane")) {
				cameraAux.nearPlane = cam["nearPlane"];
			}
			if (cam.contains("farPlane")) {
				cameraAux.farPlane = cam["farPlane"];
			}
			if (cam.contains("positionX") && cam.contains("positionY") && cam.contains("positionZ")) {
				cameraAux.positionX = cam["positionX"];
				cameraAux.positionY = cam["positionY"];
				cameraAux.positionZ = cam["positionZ"];
				camera.setPosition(glm::vec3(cameraAux.positionX, cameraAux.positionY, cameraAux.positionZ));
			}
			if (cam.contains("frontDirectionX") && cam.contains("frontDirectionY") && cam.contains("frontDirectionZ")) {
				cameraAux.frontDirectionX = cam["frontDirectionX"];
				cameraAux.frontDirectionY = cam["frontDirectionY"];
				cameraAux.frontDirectionZ = cam["frontDirectionZ"];
				camera.setFrontDirection(glm::vec3(cameraAux.frontDirectionX, cameraAux.frontDirectionY, cameraAux.frontDirectionZ));
			}
			if (cam.contains("upDirectionX") && cam.contains("upDirectionY") && cam.contains("upDirectionZ")) {
				cameraAux.upDirectionX = cam["upDirectionX"];
				cameraAux.upDirectionY = cam["upDirectionY"];
				cameraAux.upDirectionZ = cam["upDirectionZ"];
				camera.setUpDirection(glm::vec3(cameraAux.upDirectionX, cameraAux.upDirectionY, cameraAux.upDirectionZ));
			}
			camera.updateCamera();
		}
		else {
			std::cerr << "Estrutura JSON inválida: 'camera' não encontrado." << std::endl;
		}
	}

	void loadObjects() {
		for (const auto& obj : sceneObjectsAux)
		{
			SceneObj sceneObj = SceneObj(obj.x, obj.y, obj.z, obj.objFilePath, shader, obj.objectId, obj.curvePoints);
			sceneObject.push_back(sceneObj);
		}
	}
};