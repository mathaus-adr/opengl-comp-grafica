#include "lines.h"

Lines initLines()
{
    Lines lines = Lines();
	glGenVertexArrays(1, &lines.vao);
	glGenBuffers(1, &lines.vbo);
	glBindVertexArray(lines.vao);
	glBindBuffer(GL_ARRAY_BUFFER, lines.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lines.vertices), lines.vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	return lines;
}

void drawLines(Lines lines, UniformLocation uniform)
{
	float offsetX = -0.2f;
	glm::mat4 lineModel = glm::translate(glm::mat4(1.0f), glm::vec3(offsetX, 0.0f, 0.0f));
	glUniformMatrix4fv(uniform.modelLoc, 1, GL_FALSE, glm::value_ptr(lineModel));

	glBindVertexArray(lines.vao);
	glDrawArrays(GL_LINES, 0, 8);
	glBindVertexArray(0);
}
