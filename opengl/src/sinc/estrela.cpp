
//#include <cmath>



constexpr int numPoints = 5;
constexpr float M_PI = 3.14159265358979323846;
constexpr float scale = 0.2f;
constexpr float JUMP_DURATION = 1.0f;
constexpr float JUMP_HEIGHT = 0.2f;
constexpr float rightAngle =  270.0f;
constexpr float leftAngle = 270.0f;
constexpr float translationSpeed = 0.1f;
constexpr float rotationDuration = 5.0f;


#include "estrela.h"

Star initStar()
{
	Star star = Star();

	//const int numPoints = 5;
	const float angle = 2.0f * M_PI / numPoints;
	//float scale = 0.2f;  // Aumentado para melhorar a visibilidade
	float offsetAngle = M_PI / 2.0f; // Ajuste para alinhar as pernas


	for (int i = 0; i < numPoints; ++i)
	{
		float theta = i * angle + offsetAngle;
		star.vertices[i * 7] = scale * cos(theta);
		star.vertices[i * 7 + 1] = scale * sin(theta);
		star.vertices[i * 7 + 2] = 1.0f;
		star.vertices[i * 7 + 3] = 1.0f;  // Cor R
		star.vertices[i * 7 + 4] = 1.0f;  // Cor G
		star.vertices[i * 7 + 5] = 0.0f;  // Cor B (Ajustado para amarelo)
		star.vertices[i * 7 + 6] = 1.0f;  // Alpha
	}

	GLuint indices[10]; // 10 Ã­ndices para 5 linhas
	for (int i = 0; i < numPoints; ++i)
	{
		star.indices[i * 2] = i;
		star.indices[i * 2 + 1] = (i + 2) % numPoints;
	}

	glGenVertexArrays(1, &star.vao);
	glBindVertexArray(star.vao);
	glGenBuffers(1, &star.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, star.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(star.vertices), star.vertices, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &star.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, star.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(star.indices), star.indices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, star.vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	star.control.timeControl.startTime = glfwGetTime();
	return star;
}

void drawJumpingStar(Star& star, UniformLocation uniform, Transformation& transformation, float timer[4])
{
	//star.control.timeControl.elapsedTime = star.control.timeControl.startTime - glfwGetTime();
	if (!star.control.jumpControl.jumping) {
		return;
	}

	if (timer[Jumping] < 0) {
		timer[Jumping] = glfwGetTime();
	}
	
	float elapsedTime = glfwGetTime() - timer[Rotating];

	star.control.jumpControl.currentJump = static_cast<int>(elapsedTime / JUMP_DURATION);

	if (star.control.jumpControl.currentJump >= star.control.jumpControl.numJumps)
	{
		star.control.jumpControl.jumping = false;
		star.control.jumpControl.currentJump = 0;
		return;
	}

	float jumpElapsedTime = elapsedTime - star.control.jumpControl.currentJump * JUMP_DURATION;
	float jumpFraction = jumpElapsedTime / (JUMP_DURATION / 2);
	
	if (jumpFraction > 1.0f) { 
		jumpFraction = 2.0f - jumpFraction; 
	}

	float currentJumpHeight = JUMP_HEIGHT * jumpFraction;
	transformation.model = glm::translate(transformation.model, glm::vec3(0.0f, currentJumpHeight, 0.0f));

	draw(star, uniform, transformation);
}

void drawRotatingStar(Star& star, UniformLocation uniform, Transformation& transformation, float timer[4])
{
	if (star.control.jumpControl.jumping) return;

	if (!star.control.rotationControl.isRotating) return;

	if (timer[Rotating] < 0) {
		timer[Rotating] = glfwGetTime();
	}

	float elapsedTime = glfwGetTime() - timer[Rotating];

	float fraction = elapsedTime / rotationDuration;
	float rotationFraction = fmin(fraction, 1.0f);
	float currentRotationAngle = (star.control.rotationControl.movingForward ? rightAngle : leftAngle) * rotationFraction;

	if (star.control.rotationControl.movingForward)
	{
		if (currentRotationAngle >= rightAngle)
		{
			star.control.rotationControl.movingForward = false;
			timer[Rotating] = glfwGetTime();
		}
	}
	else
	{
		if (currentRotationAngle >= leftAngle)
		{
			star.control.rotationControl.movingForward = true;
			star.control.rotationControl.isRotating = false;
		}
	}

	float translation = translationSpeed * elapsedTime;

	float backTranslation = translationSpeed * (elapsedTime - rotationDuration); // TranslaÃ§Ã£o de volta
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(translation, 0.0f, 0.0f));

	glm::mat4 rotationMatrix;


	rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(currentRotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

	if (!star.control.rotationControl.movingForward) {
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-backTranslation, 0.0f, 0.0f));
	}

	transformation.model = translationMatrix * rotationMatrix;
	draw(star, uniform, transformation);
}

void draw(Star& star, UniformLocation uniform, Transformation& transformation)
{
	glUniformMatrix4fv(uniform.modelLoc, 1, GL_FALSE, glm::value_ptr(transformation.model));
	glUniformMatrix4fv(uniform.projLoc, 1, GL_FALSE, glm::value_ptr(transformation.projection));
	glUniformMatrix4fv(uniform.viewLoc, 1, GL_FALSE, glm::value_ptr(transformation.view));
	glBindVertexArray(star.vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, star.ebo);
	glDrawElements(GL_LINES, 10, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
