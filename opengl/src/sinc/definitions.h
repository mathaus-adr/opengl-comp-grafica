#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <iostream>

struct Transformation {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
};

struct UniformLocation
{
	GLint modelLoc;
	GLint projLoc;
	GLint viewLoc;
};

struct TimeControl {
	float startTime;
	float currentTime;
	float elapsedTime;
	float maxTime;
};

#ifndef PHASE_TIMER_H
#define PHASE_TIMER_H
extern float phaseTimer[4];

enum Phase {
	Jumping = 0,
	Rotating = 1,
};
#endif