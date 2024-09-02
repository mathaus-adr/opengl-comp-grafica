#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "definitions.h"

struct JumpControl {
	bool jumping = true;
	int numJumps = 4;
	int currentJump = 0;
};

struct RotationControl {
	bool isRotating = true;
	bool movingForward = true;
};

struct StarControl {
	JumpControl jumpControl;
	RotationControl rotationControl;
	TimeControl timeControl;
	bool movingForward = true;
};


struct Star {
	GLuint vao;
	GLuint ebo;
	GLuint vbo;
	GLfloat vertices[10 * 7];
	GLuint indices[10];
	StarControl control;
};

Star initStar();

void drawJumpingStar(Star& star, UniformLocation uniform, Transformation& transformation, float timer[4]);

void drawRotatingStar(Star& star, UniformLocation uniform, Transformation& transformation, float timer[4]);

void draw(Star& star, UniformLocation uniform, Transformation& transformation);