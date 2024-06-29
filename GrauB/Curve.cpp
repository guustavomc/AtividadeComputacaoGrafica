#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector> 
#include "../Common/include/Shader.h"

using namespace std;

class Curve
{
public:
	inline void setControlPoints(vector <glm::vec3> controlPoints)
	{ 
		this->controlPoints = controlPoints; 
	}
	
	void setShader(Shader* shader)
	{
		this->shader = shader;
		shader->Use();
	}

	void generateCurve(int pointsPerSegment);

	void drawCurve(glm::vec4 color)
	{
		shader->setVec4("finalColor", color.r, color.g, color.b, color.a);

		glBindVertexArray(VAO);
		// Chamada de desenho - drawcall
		// CONTORNO e PONTOS - GL_LINE_LOOP e GL_POINTS
		glDrawArrays(GL_LINE_STRIP, 0, curvePoints.size());
		//glDrawArrays(GL_POINTS, 0, curvePoints.size());
		glBindVertexArray(0);

	}

	int getNbCurvePoints() 
	{ 
		return curvePoints.size(); 
	}

	glm::vec3 getPointOnCurve(int i) 
	{ 
		return curvePoints[i]; 
	}

protected:
	vector <glm::vec3> controlPoints;
	vector <glm::vec3> curvePoints;
	glm::mat4 M; //Matriz de base
	GLuint VAO;
	Shader* shader;
};