#pragma once


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "definitions.h"
struct Lines {
	GLuint vao;
	GLuint ebo;
	GLuint vbo;
	GLfloat vertices[56] = {
		-0.22f, -0.2f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // Linha 1: Base
		-0.22f, -0.15615f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // Linha 1: Extrema superior

		-0.02f, -0.2f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // Linha 2: Base
		-0.02f, -0.1125f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // Linha 2: Extrema superior

		0.25f, -0.2f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // Linha 3: Base
		0.25f, -0.024f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // Linha 3: Extrema superior

		0.50f, -0.2f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // Linha 4: Base
		0.50f, 0.1f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f   // Linha 4: Extrema superior
	};
};



Lines initLines();

void drawLines(Lines lines, UniformLocation uniform);