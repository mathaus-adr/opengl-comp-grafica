#include <iostream>
#include <cmath>
#define _USE_MATH_DEFINES
#include <cmath>

#include <vector>
#include "definitions.h"
#include "estrela.h"
#include "rectangle.h"
#include "lines.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
// V�rtices do pent�gono
#define DEG_TO_RAD(deg) ((deg) * M_PI / 180.0)
// �ngulo de rota��o em graus
const float angulo = 3.0f;
const float radians = DEG_TO_RAD(angulo);
// Matrizes de rota��o
const float cosA = cosf(radians);
const float sinA = sinf(radians);
#endif

// Variaveis Globais
const int gWindowWidth = 800;
const int gWindowHeight = 600;
const char* TITLE = "Pentagram";
GLFWwindow* gWindow = NULL;

// Shaders
const GLchar* vertexShaderSrc =
"#version 330 core\n"
"layout (location = 0) in vec3 pos;"
"layout (location = 1) in vec4 color;"
"\n"
"out vec4 vert_color;"
"uniform mat4 projection;"
"uniform mat4 view;"
"uniform mat4 model;"
"void main()"
"{"
"    gl_Position = projection * view * model * vec4(pos, 1.0);"
"    vert_color = color;"
"}";

const GLchar* fragmentShaderSrc =
"#version 330 core\n"
"in vec4 vert_color;"
"out vec4 frag_color;"
"void main()"
"{"
"    frag_color = vert_color;"
"}";

struct Triangle {
	GLuint vao;
	GLuint ebo;
	GLuint vbo;
	GLfloat vertices[21];
	GLuint* indices;
};

struct Square {
	GLuint vao;
	GLuint ebo;
	GLuint vbo;
	GLfloat vertices[28];
	GLuint indices[8];
};

bool initOpenGL();

void DrawStar(UniformLocation uniform, Transformation transformation, Star star);

//void getUniformMatrixes(UniformLocation uniform, Transformation transformation);

void DrawInvertedTriangle(GLint modelLoc, GLint projLoc, glm::mat4& projection, GLint viewLoc, glm::mat4& view, GLuint invertedTriangleVAO, GLuint squareVAO);

void DrawInvertedTriangle(UniformLocation uniform, Transformation transformation, GLuint invertedTriangleVAO, GLuint squareVAO);

void clear(GLuint& vao, GLuint& vbo, GLuint& ebo, GLuint& lineVAO, GLuint& lineVBO, GLuint shaderProgram);

void RotateStar(bool rotatingForward, glm::mat4& rotationMatrix, float currentRotationAngle, glm::mat4& translationMatrix, float backTranslation);

void JumpStar(bool& jumping, float elapsedTime, float jumpDuration, int numJumps, float& startTime, float currentTime, float jumpHeight, Transformation& transformation);

void DrawRotatingStar(bool& finishedRotation, float elapsedTime, float rotationDuration, bool& rotatingForward, float rightRotationAngle, float leftRotationAngle, float& startTime, float currentTime, int currentLineIndex, bool& jumping, float translationSpeed, glm::mat4& model);

void DisplayInvertedTriangle(bool finishedRotation, bool& displayingInvertedTriangle, float& displayStartTime, float currentTime);

void DrawRectangles(GLint modelLoc, glm::mat4& model, GLint projLoc, glm::mat4& projection, GLint viewLoc, glm::mat4& view, GLuint rectangleVAO);

void DrawRectangles(UniformLocation uniform, Transformation transformation, Rectangle rectangle);

void DrawHexagon(GLint modelLoc, glm::mat4& model, GLint projLoc, glm::mat4& projection, GLint viewLoc, glm::mat4& view, GLuint hexagonVAO, GLuint hexagonEBO);

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


void checkShaderCompileStatus(GLuint shader)
{
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cerr << "Shader Compilation Error: " << infoLog << std::endl;
	}
}

void checkProgramLinkStatus(GLuint program)
{
	GLint success;
	GLchar infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cerr << "Program Link Error: " << infoLog << std::endl;
	}
}

int main(void)
{
	if (!initOpenGL())
	{
		std::cerr << "Initialization failed" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(gWindow, framebuffer_size_callback);
	Star star = initStar();
	Square square;
	Lines lines = initLines();

	GLfloat squareVertices[] = {
		// Posição         // Cor
		-0.15f, -0.1f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, // Inferior esquerdo
		0.15f, -0.1f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,  // Inferior direito
		0.15f, 0.1f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,   // Superior direito
		-0.15f, 0.1f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f   // Superior esquerdo
	};


	for (int i = 0; i < (sizeof(squareVertices) / sizeof(GLfloat)); i++)
	{
		square.vertices[i] = squareVertices[i];
	}

	// Índices para desenhar o quadrado
	GLuint squareIndices[] = {
		0, 1, 1, 2, 2, 3, 3, 0
	};

	for (int i = 0; i < (sizeof(squareIndices) / sizeof(GLuint)); i++)
	{
		square.indices[i] = squareIndices[i];
	}

	// Configura o VAO e VBO para o quadrado
	GLuint squareVAO, squareEBO;

	glGenVertexArrays(1, &square.vao);
	glBindVertexArray(square.vao);
	glGenBuffers(1, &square.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, square.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square.vertices), square.vertices, GL_STATIC_DRAW);
	glGenBuffers(1, &square.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, square.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(square.indices), square.indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	// Definição dos vértices para linhas verticais

	Triangle triangle;
	// Definição dos vértices do triângulo invertido
	GLfloat invertedTriangleVertices[] = {
		// Posição          // Cor
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // Vértice inferior (centralizado verticalmente)
		-0.125f, 0.1f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // Vértice superior esquerdo
		0.125f, 0.1f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f  // Vértice superior direito
	};

	for (int i = 0; i < (sizeof(invertedTriangleVertices) / sizeof(GLfloat)); i++)
	{
		triangle.vertices[i] = invertedTriangleVertices[i];
	}


	GLuint invertedTriangleVAO, invertedTriangleVBO;
	glGenVertexArrays(1, &triangle.vao);
	glGenBuffers(1, &triangle.vbo);
	glBindVertexArray(triangle.vao);
	glBindBuffer(GL_ARRAY_BUFFER, triangle.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle.vertices), triangle.vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	GLuint lineVBO, lineVAO;

	Rectangle rectangle = initRectangle();


	GLfloat fator_escala = 0.5f;

	GLfloat hexagonVertices[] = {
		// Posi��o                   // Cor
		-0.5f * fator_escala, 0.0f * fator_escala, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // V�rtice superior
		-0.25f * fator_escala, 0.433f * fator_escala, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // V�rtice superior direito
		0.25f * fator_escala, 0.433f * fator_escala, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // V�rtice inferior direito
		0.5f * fator_escala, 0.0f * fator_escala, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, // V�rtice inferior
		0.25f * fator_escala, -0.433f * fator_escala, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // V�rtice inferior esquerdo
		-0.25f * fator_escala, -0.433f * fator_escala, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f  // V�rtice superior esquerdo
	};


	GLuint hexagonIndices[] = {
		0, 1, 2, // Tri�ngulo 1
		0, 2, 3, // Tri�ngulo 2
		0, 3, 4, // Tri�ngulo 3
		0, 4, 5  // Tri�ngulo 4
	};

	GLuint hexagonVBO, hexagonVAO, hexagonEBO;
	glGenVertexArrays(1, &hexagonVAO);
	glGenBuffers(1, &hexagonVBO);
	glGenBuffers(1, &hexagonEBO);
	glBindVertexArray(hexagonVAO);

	glBindBuffer(GL_ARRAY_BUFFER, hexagonVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(hexagonVertices), hexagonVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hexagonEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(hexagonIndices), hexagonIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Desvincular o VAO

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertexShaderSrc, NULL);
	glCompileShader(vs);
	checkShaderCompileStatus(vs);

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragmentShaderSrc, NULL);
	glCompileShader(fs);
	checkShaderCompileStatus(fs);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vs);
	glAttachShader(shaderProgram, fs);
	glLinkProgram(shaderProgram);
	checkProgramLinkStatus(shaderProgram);

	glDeleteShader(vs);
	glDeleteShader(fs);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float rightRotationAngle = 270.0f;
	float leftRotationAngle = 270.0f;
	float translationSpeed = 0.1f;
	float rotationDuration = 5.0f;
	float startTime = glfwGetTime();

	// Variáveis para animação de pulo
	int numJumps = 4;
	float jumpDuration = 1.0f; // Duração de cada pulo (em segundos)
	float jumpHeight = 0.2f;   // Altura de cada pulo (ajustado para melhor visibilidade)
	bool jumping = true;

	// Variáveis para rotação e translação
	bool rotatingForward = true; // Indica se o pentagrama está rotacionando para frente
	bool movingBack = false;     // Indica se o pentagrama está movendo para trás

	// Variáveis para animação de transparência
	float transparencyStartTime = 0.0f; // Tempo de início da transparência
	int currentTransparentVertex = 0; // Índice do vértice que está se tornando transparente
	bool waitingForTransparency = false; // Se o programa está esperando a transparência
	const float transparencyWaitDuration = 2.0f; // Duração da espera em segundos

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	int currentLine = 0; // Controla a linha atual a ser desenhada
	const float lineSwitchDuration = 1.0f; // Duração de exibição de cada linha (em segundos)
	static int currentLineIndex = 4; // Índice da linha atual a ser desenhada
	static float lineSwitchTime = 1.0f; // Tempo para alternar entre linhas
	static int atual = 0;

	// Código principal simplificado com ajustes para manter linhas visíveis

	bool finishedRotation = false; // Adicione esta variável
	// Variáveis para transformação final
	bool transformingToTriangle = false; // Indica se a transformação para o triângulo está ocorrendo
	float triangleTransformationDuration = 3.0f; // Duração da transformação
	float triangleStartTime = 0.0f; // Tempo de início da transformação

	bool drawingTriangle = false; // Flag para saber se devemos desenhar o triângulo

	float triangleDisplayStartTime = 0.0f;
	bool displayingTriangleAndSquare = false;
	// Adicione uma nova variável para controlar o tempo de exibição do triângulo e quadrado
	bool displayingInvertedTriangle = false;
	float displayStartTime = 0.0f;
	int jumpCount = 0;
	Transformation transformation;
	UniformLocation uniform;

	//float timer[] = {-1, -1};
	float phaseTimer[4] = { -1.0f, -1.0f, -1.0f, -1.0f };
	while (!glfwWindowShouldClose(gWindow))
	{
		float currentTime = glfwGetTime();
		float elapsedTime = currentTime - startTime;
		std::cout << "Tempo decorrido: " << elapsedTime << std::endl;
		glm::mat4 model = transformation.model = glm::mat4(1.0f);

		// Configura a proje��o e a visualiza��o
		int width, height;
		glfwGetFramebufferSize(gWindow, &width, &height);
		transformation.projection = glm::ortho(-1.0f, 1.0f, -1.0f * (float)height / width, 1.0f * (float)height / width, -1.0f, 1.0f);
		transformation.view = glm::mat4(1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);

		uniform.projLoc = glGetUniformLocation(shaderProgram, "projection");
		uniform.viewLoc = glGetUniformLocation(shaderProgram, "view");
		uniform.modelLoc = glGetUniformLocation(shaderProgram, "model");

		if (uniform.projLoc == -1 || uniform.viewLoc == -1 || uniform.modelLoc == -1)
		{
			std::cerr << "Uniform Location Error" << std::endl;
		}

		// Anima��o de pulo

		drawJumpingStar(star, uniform, transformation, phaseTimer);
		drawRotatingStar(star, uniform, transformation, phaseTimer);
		DisplayInvertedTriangle(finishedRotation, displayingInvertedTriangle, displayStartTime, currentTime);

		if (!star.control.rotationControl.isRotating)
		{
			drawLines(lines, uniform);
		}

		//else if (displayingInvertedTriangle)
		//{
		//	if (currentTime - displayStartTime < 3.0f)
		//	{
		//		// Desenha o tri�ngulo e o quadrado por 3 segundos
		DrawInvertedTriangle(uniform, transformation, triangle.vao, square.vao);
		//	}
		//	else if (star.control.jumpControl.currentJump < 3)
		//	{
		//		if (!star.control.jumpControl.jumping)
		//		{
		//			// Inicia um novo pulo
		//			jumping = true;
		//			//star.control.timeControl.startTime = currentTime;
		//			jumpCount++;  // Incrementa o contador de pulos
		//		}

		//		// Calcula o tempo decorrido do pulo
		//		float jumpElapsedTime = currentTime - startTime;
		//		float jumpFraction = jumpElapsedTime / (jumpDuration / 2);
		//		if (jumpFraction > 1.0f) jumpFraction = 2.0f - jumpFraction;
		//		float currentJumpHeight = jumpHeight * jumpFraction;

		//		// Atualiza a matriz de modelo para refletir a altura do pulo
		//		glm::mat4 modelJump = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, currentJumpHeight, 0.0f));

		//		drawJumpingStar(star, uniform, transformation, phaseTimer);

		//		// Verifica se o pulo terminou
		//		if (jumpElapsedTime > jumpDuration)
		//		{
		//			jumping = false;
		//		}
		//	}
		//	else
		//	{
		//		//DrawStar(uniform, transformation, star);
		//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//		std::cout << "Desenhando estrela" << std::endl;
		//		//drawRectangle(rectangle, uniform, transformation);
		//	}
		//	//DrawHexagon(modelLoc, model, projLoc, projection, viewLoc, view, hexagonVAO, hexagonEBO);
		//}
		glfwSwapBuffers(gWindow);
		glfwPollEvents();
	}

	clear(star.vao, star.vbo, star.ebo, lineVAO, lineVBO, shaderProgram);
	glfwDestroyWindow(gWindow);
	glfwTerminate();
	return 0;
}

void DisplayInvertedTriangle(bool finishedRotation, bool& displayingInvertedTriangle, float& displayStartTime, float currentTime)
{
	if (finishedRotation)
	{
		// Inicia o cronômetro para exibir o triângulo e o quadrado
		if (!displayingInvertedTriangle)
		{
			displayStartTime = currentTime;
			displayingInvertedTriangle = true;
		}
	}
}

void clear(GLuint& vao, GLuint& vbo, GLuint& ebo, GLuint& lineVAO, GLuint& lineVBO, GLuint shaderProgram)
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &lineVAO);
	glDeleteBuffers(1, &lineVBO);
	glDeleteProgram(shaderProgram);
}

void DrawInvertedTriangle(GLint modelLoc, GLint projLoc, glm::mat4& projection, GLint viewLoc, glm::mat4& view, GLuint invertedTriangleVAO, GLuint squareVAO)
{
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	glBindVertexArray(invertedTriangleVAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

	glBindVertexArray(squareVAO);
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void DrawInvertedTriangle(UniformLocation uniform, Transformation transformation, GLuint invertedTriangleVAO, GLuint squareVAO)
{
	glUniformMatrix4fv(uniform.modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glUniformMatrix4fv(uniform.projLoc, 1, GL_FALSE, glm::value_ptr(transformation.projection));
	glUniformMatrix4fv(uniform.viewLoc, 1, GL_FALSE, glm::value_ptr(transformation.view));

	glBindVertexArray(invertedTriangleVAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

	glBindVertexArray(squareVAO);
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}



void DrawStar(UniformLocation uniform, Transformation transformation, Star star)
{
	//getUniformMatrixes(uniform, transformation);
	glBindVertexArray(star.vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, star.ebo);
	glDrawElements(GL_LINES, 10, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}




void DrawRectangles(GLint modelLoc, glm::mat4& model, GLint projLoc, glm::mat4& projection, GLint viewLoc, glm::mat4& view, GLuint rectangleVAO)
{
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	glBindVertexArray(rectangleVAO);

	// Define o modelo para desenhar os ret�ngulos
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void DrawRectangles(UniformLocation uniform, Transformation transformation, Rectangle rectangle)
{
	//getUniformMatrixes(uniform, transformation);
	glBindVertexArray(rectangle.vao);
	glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

// Fun��o para desenhar o hezagono
void DrawHexagon(GLint modelLoc, glm::mat4& model, GLint projLoc, glm::mat4& projection, GLint viewLoc, glm::mat4& view, GLuint hexagonVAO, GLuint hexagonEBO) {
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(hexagonVAO);
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0); // 18 �ndices para o hex�gono
	glBindVertexArray(0);
}



bool initOpenGL()
{
	// Inicializa a biblioteca GLFW
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return false;
	}

	// Configura GLFW para usar OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Cria a janela GLFW
	gWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, TITLE, NULL, NULL);
	if (!gWindow)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(gWindow);

	// Inicializa GLEW
	glewExperimental = GL_TRUE; // Permite uso de funções modernas do OpenGL
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return false;
	}

	// Configura o viewport
	glViewport(0, 0, gWindowWidth, gWindowHeight);
	return true;
}