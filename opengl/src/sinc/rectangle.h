#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "definitions.h"

struct Rectangle
{
	GLuint vao;
	GLuint ebo;
	GLuint vbo;
	GLfloat vertices[140] = {
		// Retângulo vermelho movido 0,01 para cima, 0,01 para a direita e mais 0,01 para a direita
		0.114f, -0.015f, -0.01f,  1.0f, 1.0f, 0.0f, 1.0f,
		0.255f, 0.127f, -0.01f,  1.0f, 1.0f, 0.0f, 1.0f,
		0.184f, 0.198f, -0.01f,  1.0f, 1.0f, 0.0f, 1.0f,
		0.043f, 0.056f, -0.01f,  1.0f, 1.0f, 0.0f, 1.0f,

		// Retângulo verde movido 0,02 para a esquerda, 0,01 para cima, 0,001 para a esquerda e mais 0,01 para a esquerda
		-0.254f, 0.127f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,
		-0.114f, -0.014f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,
		-0.044f, 0.056f, 0.0f,   1.0f, 1.0f, 0.0f, 1.0f,
		-0.184f, 0.197f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,

		// Retângulo 3 azul
		-0.05f, 0.06f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,
		0.05f, 0.06f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,
		0.05f, 0.26f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,
		-0.05f, 0.26f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,

		// Retângulo 4 amarelo rotacionado, movido 0,01 para cima, 0,01 para a direita e 0,02 para a esquerda
		0.156f, -0.226f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,
		0.014f, -0.084f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,
		0.084f, -0.014f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,
		0.226f, -0.156f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,

		// Retângulo 5 roxo movido 0,01 para cima
		-0.212f, -0.155f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,
		-0.07f, -0.013f, 0.0f,   1.0f, 1.0f, 0.0f, 1.0f,
		0.001f, -0.083f, 0.0f,   1.0f, 1.0f, 0.0f, 1.0f,
		-0.141f, -0.225f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,
	};
	GLuint indices[30] = {
		// Retângulo 1
		0, 1, 2, 2, 3, 0,
		// Retângulo 2
		4, 5, 6, 6, 7, 4,
		// Retângulo 3
		8, 9, 10, 10, 11, 8,
		// Retângulo 4
		12, 13, 14, 14, 15, 12,
		// Retângulo 5
		16, 17, 18, 18, 19, 16,
	};
};


Rectangle initRectangle();

void drawRectangle(Rectangle rec, UniformLocation uniform, Transformation transformation);