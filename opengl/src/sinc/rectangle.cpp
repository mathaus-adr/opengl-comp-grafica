#include "rectangle.h"

Rectangle initRectangle()
{
    Rectangle rec = Rectangle();

	glGenVertexArrays(1, &rec.vao);
	glGenBuffers(1, &rec.vbo);
	glGenBuffers(1, &rec.ebo);
	glBindVertexArray(rec.vao);
	glBindBuffer(GL_ARRAY_BUFFER, rec.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rec.vertices), &rec.vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rec.vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rec.indices), &rec.indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	return rec;
}

void drawRectangle(Rectangle rec, UniformLocation uniform, Transformation transformation)
{
	glUniformMatrix4fv(uniform.projLoc, 1, GL_FALSE, glm::value_ptr(transformation.projection));
	glUniformMatrix4fv(uniform.viewLoc, 1, GL_FALSE, glm::value_ptr(transformation.view));
	glUniformMatrix4fv(uniform.modelLoc, 1, GL_FALSE, glm::value_ptr(transformation.model));
	glBindVertexArray(rec.vao);
	glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
