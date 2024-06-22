#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Common/include/Shader.h"

class Camera
{
public:
	Camera(Shader* shader, int width, int height, float fov = glm::radians(45.0f), float nearPlane = 0.1f,
		float farPlane = 100.0f, float mouseSensitivity = 0.05, float pitch = 0.0, float yaw = -90.0, float cameraSpeed = 0.05,
		float minFov = 0.5f, float maxFov = 10.0f, float scrollSpeed = 0.05f)
		: shader(shader), width(width), height(height), fov(fov), nearPlane(nearPlane), farPlane(farPlane),
		mouseSensitivity(mouseSensitivity), pitch(pitch), yaw(yaw), cameraSpeed(cameraSpeed), minFov(minFov), 
		maxFov(maxFov), scrollSpeed(scrollSpeed)
	{
		this->lastMouseX = width / 2.0;
		this->lastMouseY = height / 2.0;
		this->aspectRatio = (float)width / (float)height;
		setProjection();
	}

	void setProjection()
	{
		glm::mat4 projection = glm::perspective(fov, aspectRatio, nearPlane, farPlane);
		shader->setMat4("projection", glm::value_ptr(projection));
	}

	void updateViewMatrix()
	{
		glm::mat4 view = glm::lookAt(position, position + frontDirection, upDirection);
		shader->setMat4("view", glm::value_ptr(view));
	}

	void updateCameraPosition() {
		shader->setVec3("camera_pos", position.x, position.y, position.z);
	}

	void updateCamera()
	{
		updateViewMatrix();
		updateCameraPosition();
	}

	void scrollCamera(double yOffset) {
		fov -= (yOffset * scrollSpeed);
		fov = glm::clamp(fov, minFov, maxFov);
		setProjection();
	}

	void moveCamera(int key) {
		switch (key)
		{
		case(GLFW_KEY_W):
			position += frontDirection * cameraSpeed;
			break;
		case(GLFW_KEY_S):
			position -= frontDirection * cameraSpeed;
			break;
		case(GLFW_KEY_A):
			position -= glm::normalize(glm::cross(frontDirection, upDirection)) * cameraSpeed;
			break;
		case(GLFW_KEY_D):
			position += glm::normalize(glm::cross(frontDirection, upDirection)) * cameraSpeed;
			break;
		case(GLFW_KEY_C):
			position = glm::vec3(0.0, 0.0, 5.0);;
			break;
		default:
			break;
		}
	}

	void updateCameraDirection(double newMouseX, double newMouseY)
	{
		if (firstMouseMovement)
		{
			lastMouseX = newMouseX;
			lastMouseY = newMouseY;
			firstMouseMovement = false;
		}

		float offsetX = newMouseX - lastMouseX;
		float offsetY = lastMouseY - newMouseY;

		lastMouseX = newMouseX;
		lastMouseY = newMouseY;

		offsetX *= mouseSensitivity;
		offsetY *= mouseSensitivity;

		pitch += offsetY;
		yaw += offsetX;

		const float pitchLimit = 89.0f;
		pitch = glm::clamp(pitch, -pitchLimit, pitchLimit);

		updateDirectionVectors();
	}

	void updateDirectionVectors() {
		glm::vec3 newFront;
		newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		newFront.y = sin(glm::radians(pitch));
		newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		frontDirection = glm::normalize(newFront);

		const glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 rightDirection = glm::normalize(glm::cross(frontDirection, worldUp));
		upDirection = glm::normalize(glm::cross(rightDirection, frontDirection));
	}

protected:
	Shader* shader;
	int width, height;

	glm::vec3 position = glm::vec3(0.0, 0.0, 5.0);
	glm::vec3 frontDirection = glm::vec3(0.0, 0.0, -1.0);
	glm::vec3 upDirection = glm::vec3(0.0, 1.0, 0.0);

	bool firstMouseMovement = true;
	float lastMouseX, lastMouseY, mouseSensitivity, pitch, yaw;
	float aspectRatio, fov, nearPlane, farPlane;

	float cameraSpeed;

	const float minFov, maxFov, scrollSpeed;
};