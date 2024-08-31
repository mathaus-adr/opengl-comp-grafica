#include <iostream>
#include <cmath>
#define _USE_MATH_DEFINES
#include <cmath>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
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

struct Line {
	glm::vec3 position;
	glm::vec4 color;
};

struct Lines {
	std::vector<Line> lines;
} lines;


bool initOpenGL();

void DrawStar(GLint modelLoc, glm::mat4& model, GLint projLoc, glm::mat4& projection, GLint viewLoc, glm::mat4& view, GLuint vao, GLuint ebo);

void DrawInvertedTriangle(GLint modelLoc, GLint projLoc, glm::mat4& projection, GLint viewLoc, glm::mat4& view, GLuint invertedTriangleVAO, GLuint squareVAO);

void DrawRedLines(GLint modelLoc, GLuint lineVAO, int currentLineIndex);

void clear(GLuint& vao, GLuint& vbo, GLuint& ebo, GLuint& lineVAO, GLuint& lineVBO, GLuint shaderProgram);

void RotateStar(bool rotatingForward, glm::mat4& rotationMatrix, float currentRotationAngle, glm::mat4& translationMatrix, float backTranslation);

void JumpStar(bool& jumping, float elapsedTime, float jumpDuration, int numJumps, float& startTime, float currentTime, float jumpHeight, glm::mat4& model);

void DrawRotatingStar(bool& finishedRotation, float elapsedTime, float rotationDuration, bool& rotatingForward, float rightRotationAngle, float leftRotationAngle, float& startTime, float currentTime, int currentLineIndex, bool& jumping, float translationSpeed, glm::mat4& model);

void DisplayInvertedTriangle(bool finishedRotation, bool& displayingInvertedTriangle, float& displayStartTime, float currentTime);

void generateLines(GLuint& lineVAO, GLuint& lineVBO);

void DrawRectangles(GLint modelLoc, glm::mat4& model, GLint projLoc, glm::mat4& projection, GLint viewLoc, glm::mat4& view, GLuint rectangleVAO);

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

	// Calcula os vértices do pentagrama
	const int numPoints = 5;
	const float angle = 2.0f * M_PI / numPoints;
	GLfloat vertices[10 * 7]; // Cada ponto tem 7 valores (posição e cor com alpha)
	float scale = 0.2f;  // Aumentado para melhorar a visibilidade
	float offsetAngle = M_PI / 2.0f; // Ajuste para alinhar as pernas

	// Gera os pontos do pentagrama
	// Gera os pontos do pentagrama
	for (int i = 0; i < numPoints; ++i)
	{
		float theta = i * angle + offsetAngle;
		vertices[i * 7] = scale * cos(theta);
		vertices[i * 7 + 1] = scale * sin(theta);
		vertices[i * 7 + 2] = 1.0f;
		vertices[i * 7 + 3] = 1.0f;  // Cor R
		vertices[i * 7 + 4] = 1.0f;  // Cor G
		vertices[i * 7 + 5] = 0.0f;  // Cor B (Ajustado para amarelo)
		vertices[i * 7 + 6] = 1.0f;  // Alpha
	}

	GLuint indices[10]; // 10 índices para 5 linhas
	for (int i = 0; i < numPoints; ++i)
	{
		indices[i * 2] = i;
		indices[i * 2 + 1] = (i + 2) % numPoints;
	}

	GLuint vbo, vao, ebo;
	glGenBuffers(1, &vbo);

	// destaca
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	glEnableVertexAttribArray(1);
	// Definição dos vértices do quadrado
	GLfloat squareVertices[] = {
		// Posição         // Cor
		-0.15f, -0.1f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, // Inferior esquerdo
		0.15f, -0.1f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,  // Inferior direito
		0.15f, 0.1f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,   // Superior direito
		-0.15f, 0.1f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f   // Superior esquerdo
	};

	// Índices para desenhar o quadrado
	GLuint squareIndices[] = {
		0, 1, 1, 2, 2, 3, 3, 0
	};

	// Configura o VAO e VBO para o quadrado
	GLuint squareVAO, squareVBO, squareEBO;
	glGenVertexArrays(1, &squareVAO);
	glGenBuffers(1, &squareVBO);
	glGenBuffers(1, &squareEBO);
	glBindVertexArray(squareVAO);
	glBindBuffer(GL_ARRAY_BUFFER, squareVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, squareEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(squareIndices), squareIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	// Definição dos vértices para linhas verticais
	

	// Definição dos vértices do triângulo invertido
	GLfloat invertedTriangleVertices[] = {
		// Posição          // Cor
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // Vértice inferior (centralizado verticalmente)
		-0.125f, 0.1f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // Vértice superior esquerdo
		0.125f, 0.1f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f  // Vértice superior direito
	};

	GLuint invertedTriangleVAO, invertedTriangleVBO;
	glGenVertexArrays(1, &invertedTriangleVAO);
	glGenBuffers(1, &invertedTriangleVBO);
	glBindVertexArray(invertedTriangleVAO);
	glBindBuffer(GL_ARRAY_BUFFER, invertedTriangleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(invertedTriangleVertices), invertedTriangleVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	GLuint lineVBO, lineVAO;
	GLuint lineVBOduplicated, lineVAOduplicated;
	generateLines(lineVAO, lineVBO, lineVBOduplicated, lineVAOduplicated);

	GLfloat rectangleVertices[] =
	{
		// Ret�ngulo vermelho movido 0,01 para cima, 0,01 para a direita e mais 0,01 para a direita
		0.114f, -0.015f, -0.01f,  1.0f, 1.0f, 0.0f, 1.0f,
		0.255f, 0.127f, -0.01f,  1.0f, 1.0f, 0.0f, 1.0f,
		0.184f, 0.198f, -0.01f,  1.0f, 1.0f, 0.0f, 1.0f,
		0.043f, 0.056f, -0.01f,  1.0f, 1.0f, 0.0f, 1.0f,

		// Ret�ngulo verde movido 0,02 para a esquerda, 0,01 para cima, 0,001 para a esquerda e mais 0,01 para a esquerda
		-0.254f, 0.127f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,
		-0.114f, -0.014f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,
		-0.044f, 0.056f, 0.0f,   1.0f, 1.0f, 0.0f, 1.0f,
		-0.184f, 0.197f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,

		// Ret�ngulo 3 azul
		-0.05f, 0.06f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,
		0.05f, 0.06f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,
		0.05f, 0.26f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,
		-0.05f, 0.26f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,

		// Ret�ngulo 4 amarelo rotacionado, movido 0,01 para cima, 0,01 para a direita e 0,02 para a esquerda
		0.156f, -0.226f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,
		0.014f, -0.084f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,
		0.084f, -0.014f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,
		0.226f, -0.156f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,

		// Ret�ngulo 5 roxo movido 0,01 para cima
		-0.212f, -0.155f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,
		-0.07f, -0.013f, 0.0f,   1.0f, 1.0f, 0.0f, 1.0f,
		0.001f, -0.083f, 0.0f,   1.0f, 1.0f, 0.0f, 1.0f,
		-0.141f, -0.225f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,
	};


	GLuint rectangleIndices[] = {
		// Ret�ngulo 1
		0, 1, 2, 2, 3, 0,
		// Ret�ngulo 2
		4, 5, 6, 6, 7, 4,
		// Ret�ngulo 3
		8, 9, 10, 10, 11, 8,
		// Ret�ngulo 4
		12, 13, 14, 14, 15, 12,
		// Ret�ngulo 5
		16, 17, 18, 18, 19, 16,
	};

	GLuint rectangleVBO, rectangleVAO, rectangleEBO;
	glGenVertexArrays(1, &rectangleVAO);
	glGenBuffers(1, &rectangleVBO);
	glGenBuffers(1, &rectangleEBO);
	glBindVertexArray(rectangleVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rectangleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectangleEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectangleIndices), rectangleIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

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

	float rightRotationAngle = 90.0f;
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

	while (!glfwWindowShouldClose(gWindow))
	{
		float currentTime = glfwGetTime();
		float elapsedTime = currentTime - startTime;
		glm::mat4 model = glm::mat4(1.0f);

		// Configura a proje��o e a visualiza��o
		int width, height;
		glfwGetFramebufferSize(gWindow, &width, &height);
		glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f * (float)height / width, 1.0f * (float)height / width, -1.0f, 1.0f);
		glm::mat4 view = glm::mat4(1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);
		GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
		GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
		GLint modelLoc = glGetUniformLocation(shaderProgram, "model");

		if (projLoc == -1 || viewLoc == -1 || modelLoc == -1)
		{
			std::cerr << "Uniform Location Error" << std::endl;
		}

		// Anima��o de pulo
		JumpStar(jumping, elapsedTime, jumpDuration, numJumps, startTime, currentTime, jumpHeight, model);

		DrawRotatingStar(finishedRotation, elapsedTime, rotationDuration, rotatingForward, rightRotationAngle, leftRotationAngle, startTime, currentTime, currentLineIndex, jumping, translationSpeed, model);
		DisplayInvertedTriangle(finishedRotation, displayingInvertedTriangle, displayStartTime, currentTime);

		if (!drawingTriangle && !displayingInvertedTriangle)
		{
			// Desenha o pentagrama e as linhas
			DrawStar(modelLoc, model, projLoc, projection, viewLoc, view, vao, ebo);

			if (!rotatingForward)
			{
				DrawRedLines(modelLoc, lineVAO, currentLineIndex);
			}
		}
		else if (displayingInvertedTriangle)
		{
			if (currentTime - displayStartTime < 3.0f)
			{
				// Desenha o tri�ngulo e o quadrado por 3 segundos
				DrawInvertedTriangle(modelLoc, projLoc, projection, viewLoc, view, invertedTriangleVAO, squareVAO);
			}
			else if (jumpCount < 3)
			{
				if (!jumping)
				{
					// Inicia um novo pulo
					jumping = true;
					startTime = currentTime;
					jumpCount++;  // Incrementa o contador de pulos
				}

				// Calcula o tempo decorrido do pulo
				float jumpElapsedTime = currentTime - startTime;
				float jumpFraction = jumpElapsedTime / (jumpDuration / 2);
				if (jumpFraction > 1.0f) jumpFraction = 2.0f - jumpFraction;
				float currentJumpHeight = jumpHeight * jumpFraction;

				// Atualiza a matriz de modelo para refletir a altura do pulo
				glm::mat4 modelJump = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, currentJumpHeight, 0.0f));

				DrawStar(modelLoc, modelJump, projLoc, projection, viewLoc, view, vao, ebo);

				// Verifica se o pulo terminou
				if (jumpElapsedTime > jumpDuration)
				{
					jumping = false;
				}
			}

			else
			{
				// Desenha a estrela
				DrawStar(modelLoc, model, projLoc, projection, viewLoc, view, vao, ebo);
				// Desenha os 5 ret�ngulos
				DrawRectangles(modelLoc, model, projLoc, projection, viewLoc, view, rectangleVAO);
			}
			//DrawHexagon(modelLoc, model, projLoc, projection, viewLoc, view, hexagonVAO, hexagonEBO);
		}
		glfwSwapBuffers(gWindow);
		glfwPollEvents();
	}

	clear(vao, vbo, ebo, lineVAO, lineVBO, shaderProgram);
	glfwDestroyWindow(gWindow);
	glfwTerminate();
	return 0;
}

void generateLines(GLuint& lineVAO, GLuint& lineVBO, GLuint& lineVAOduplicated, GLuint& liveVBOduplicated)
{
	std::vector<GLfloat> lineData{};

	GLfloat lineVertices1[] = {
		-0.22f, -0.2f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // Linha 1: Base
		-0.22f, -0.15615f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // Linha 1: Extrema superior

		-0.22f, -0.2f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // Linha 1-duplicated: Base
		-0.22f, -0.15615f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // Linha 1-duplicated: Extrema superior

		-0.02f, -0.2f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // Linha 2: Base
		-0.02f, -0.1125f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // Linha 2: Extrema superior
		
		-0.02f, -0.2f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // Linha 2-duplicated: Base
		-0.02f, -0.1125f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // Linha 2-duplicated: Extrema superior

		0.25f, -0.2f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // Linha 3: Base
		0.25f, -0.024f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // Linha 3: Extrema superior

		0.25f, -0.2f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // Linha 3-duplicated: Base
		0.25f, -0.024f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // Linha 3-duplicated: Extrema superior

		0.50f, -0.2f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // Linha 4: Base
		0.50f, 0.1f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,   // Linha 4: Extrema 
	};
	

	for (int i = 0; i < sizeof(lineVertices1) / sizeof(GLfloat); i+=7) {
		lineData.push_back(lineVertices1[i]); 
		lineData.push_back(lineVertices1[i+1]); 
		lineData.push_back(lineVertices1[i+2]); 
		lineData.push_back(lineVertices1[i+3]); 		
		lineData.push_back(lineVertices1[i+4]); 
		lineData.push_back(lineVertices1[i+5]); 
		lineData.push_back(lineVertices1[i+6]); 

		lines.lines.push_back({
			glm::vec3(lineVertices1[i], lineVertices1[i+1], lineVertices1[i+2]),
			glm::vec4(lineVertices1[i+3], lineVertices1[i+4], lineVertices1[i+5], lineVertices1[i+6])
		});
	}

	glGenVertexArrays(1, &lineVAO);
	glBindVertexArray(lineVAO);
	glGenBuffers(1, &lineVBO);
	glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
	glBufferData(GL_ARRAY_BUFFER, lineData.size() * sizeof(GLfloat), lineData.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
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

void DrawRotatingStar(bool& finishedRotation, float elapsedTime, float rotationDuration, bool& rotatingForward, float rightRotationAngle, float leftRotationAngle, float& startTime, float currentTime, int currentLineIndex, bool& jumping, float translationSpeed, glm::mat4& model)
{
	if (jumping) return;

	if (!finishedRotation)
	{
		// Animação de rotação e translação
		float rotationFraction = fmin(elapsedTime / rotationDuration, 1.0f);
		float currentRotationAngle = (rotatingForward ? rightRotationAngle : leftRotationAngle) * rotationFraction;

		if (rotatingForward)
		{
			if (currentRotationAngle >= rightRotationAngle)
			{
				rotatingForward = false; // Muda para rotação para trás
				startTime = currentTime; // Reinicia o tempo para a rotação para trás
			}
		}
		else
		{
			if (currentRotationAngle >= leftRotationAngle)
			{
				rotatingForward = true;
				startTime = currentTime; // Reinicia o tempo para a rotação para frente

				if (currentLineIndex >= 3)
				{
					finishedRotation = true;
					jumping = true;  // Faz a estrela pular novamente no final
					startTime = currentTime; // Reinicia o tempo para o pulo final
				}
			}
		}

		float translation = translationSpeed * elapsedTime;

		float backTranslation = translationSpeed * (elapsedTime - rotationDuration); // Translação de volta
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(translation, 0.0f, 0.0f));

		glm::mat4 rotationMatrix;

		RotateStar(rotatingForward, rotationMatrix, currentRotationAngle, translationMatrix, backTranslation);

		model = translationMatrix * rotationMatrix;

	}
}

void JumpStar(bool& jumping, float elapsedTime, float jumpDuration, int numJumps, float& startTime, float currentTime, float jumpHeight, glm::mat4& model)
{
	if (jumping)
	{
		int currentJump = static_cast<int>(elapsedTime / jumpDuration);
		if (currentJump >= numJumps)
		{
			jumping = false;
			startTime = currentTime; // Reinicia o tempo para a rotação
		}
		else
		{
			float jumpElapsedTime = elapsedTime - currentJump * jumpDuration;
			float jumpFraction = jumpElapsedTime / (jumpDuration / 2);
			if (jumpFraction > 1.0f) jumpFraction = 2.0f - jumpFraction;
			float currentJumpHeight = jumpHeight * jumpFraction;
			model = glm::translate(model, glm::vec3(0.0f, currentJumpHeight, 0.0f));
		}
	}
}

void RotateStar(bool rotatingForward, glm::mat4& rotationMatrix, float currentRotationAngle, glm::mat4& translationMatrix, float backTranslation)
{
	if (rotatingForward)
	{
		rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-currentRotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else
	{
		rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(currentRotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-backTranslation, 0.0f, 0.0f));
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

void DrawRedLines(GLint modelLoc, GLuint lineVAO, int currentLineIndex)
{
	float offsetX = -0.2f;
	glm::mat4 lineModel = glm::translate(glm::mat4(1.0f), glm::vec3(offsetX, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(lineModel));

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 14);
	glBindVertexArray(0);
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

void DrawStar(GLint modelLoc, glm::mat4& model, GLint projLoc, glm::mat4& projection, GLint viewLoc, glm::mat4& view, GLuint vao, GLuint ebo)
{
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
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