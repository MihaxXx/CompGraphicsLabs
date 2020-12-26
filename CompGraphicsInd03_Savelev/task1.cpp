#include <GL/glew.h>
#include <gl/GL.h>   // GL.h header file    
#include <gl/GLU.h> // GLU.h header file     
#include <gl/freeglut.h>
#include <gl/glaux.h>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "GL/SOIL.h"
#include "GLShader.h"
using namespace std;


GLint Unif_matrix;

glm::mat4 Matrix_projection;

GLShader glShader;

//! Вершина 
struct vertex
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

//! Проверка ошибок OpenGL, если есть то вывод в консоль тип ошибки 
void checkOpenGLerror()
{
	GLenum errCode;
	if ((errCode = glGetError()) != GL_NO_ERROR)
		std::cout << "OpenGl error! - " << gluErrorString(errCode) << "\n";
}

//! Инициализация шейдеров 
void initShader()
{
	glShader.loadFiles("shaders/vertex.txt", "shaders/fragment.txt");
	checkOpenGLerror();
}

// Load and create a texture 
GLuint texture;
GLuint texture2;
void text()
{
	// Текстура 1
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height;
	unsigned char* image = SOIL_load_image("img/list.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); 

	// Текстура 2
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	image = SOIL_load_image("img/awesomeface.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint VBO, VAO, EBO;
//! Инициализация VBO 
void initVBO()
{
	GLfloat vertices[] = {

		// лицевая грань
		// Positions          // Colors           // Texture Coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, //0 = 7 // Top Right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, //1 = 8 // Bottom Right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, //2 = 12 // Bottom Left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f, //3 = 11 // Top Left 

		// верхняя крышка
		 // Positions          // Colors           // Texture Coords
		 0.5f,  0.5f, -1.0f,   1.0f, 0.5f, 0.0f,   1.0f, 0.0f, //4
		-0.5f,  0.5f, -1.0f,   1.0f, 1.0f, 0.5f,   0.0f, 0.0f, //5

		// правая боковая крышка
		0.5f,  -0.5f, -1.0f,   1.0f, 0.5f, 0.5f,  0.0f, 0.0f, //6 = 10
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, //7 = 0
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  0.0f, 1.0f, //8 = 1

		// Дальняя крышка
		-0.5f,  -0.5f, -1.0f,   1.0f, 0.0f, 1.0f,  0.0f, 1.0f,  // 9
		0.5f,  -0.5f, -1.0f,    1.0f, 0.5f, 0.5f,  1.0f, 1.0f,// 10 = 6

		// Левая крышка 
		-0.5f, 0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // 11 = 3
		-0.5f,  -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,     1.0f, 1.0f, // 12 = 2
	};
	GLuint indices[] = { 
		// лицевая грань
		0, 1, 3, 
		1, 2, 3,  

		// верхняя крышка
		0, 3, 5, 
		0, 4, 5, 

		// правая крышка
		7, 8, 6,
		7, 6, 4,

		// дальняя крышка
		5, 9, 10,
		5, 10, 4,

		// Нижняя крышка
		2, 9, 1,
		9, 10, 1,

		//левая крышка
		11, 12, 9,
		11, 9, 5
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	checkOpenGLerror();
}



//! Освобождение буфера
void freeVBO()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &VAO);

}
double angle_x = 0;

void resizeWindow(int width, int height)
{
	glViewport(0, 0, width, height);
}


//! Отрисовка 
void render()
{
	angle_x += 0.0007;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::mat4 rotate_x = { 1.0f, 0.0f, 0.0f, 0.0f,
						   0.0f, glm::cos(angle_x), -glm::sin(angle_x), 0.0f,
						   0.0f, glm::sin(angle_x), glm::cos(angle_x), 0.0f,
						   0.0f, 0.0f, 0.0f, 1.0f };

	glm::mat4 rotate_y = { glm::cos(angle_x), 0.0f, glm::sin(angle_x), 0.0f,
					   0.0f, 1, 0, 0.0f,
					   -glm::sin(angle_x),0, glm::cos(angle_x), 0.0f,
					   0.0f, 0.0f, 0.0f, 1.0f };

	Matrix_projection = Projection * View * rotate_y * rotate_x;

	//! Устанавливаем шейдерную программу текущей 
	//glUseProgram(Program);
	glShader.use();
	//glUniformMatrix4fv(Unif_matrix, 1, GL_FALSE, &Matrix_projection[0][0]);
	glShader.setUniform(glShader.getUniformLocation("matrix"), Matrix_projection);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glShader.setUniform(glShader.getUniformLocation("ourTexture1"), 0);
	//glUniform1i(glGetUniformLocation(Program, "ourTexture1"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glShader.setUniform(glShader.getUniformLocation("ourTexture2"), 1);
//	glUniform1i(glGetUniformLocation(Program, "ourTexture2"), 1);

	// Draw container
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glFlush();

	//checkOpenGLerror();

	glutSwapBuffers();

	//! Отключаем шейдерную программу  
	glUseProgram(0);
}


int main(int argc, char** argv)
{
	setlocale(0, "");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE);
	glutInitWindowSize(1000, 800);
	glutCreateWindow("Simple shaders");
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//! Обязательно перед инициализацией шейдеров 
	GLenum glew_status = glewInit();
	if (GLEW_OK != glew_status)
	{
		//! GLEW не проинициализировалась  	 	
		std::cout << "Error: " << glewGetErrorString(glew_status) << "\n";
		return 1;
	}

	//! Проверяем доступность OpenGL 2.0  	
	if (!GLEW_VERSION_2_0)
	{
		//! OpenGl 2.0 оказалась не доступна  	 	
		std::cout << "No support for OpenGL 2.0 found\n";
		return 1;
	}

	//! Инициализация  
	glClearColor(0.5, 0.5, 0.5, 0);

	text();
	initVBO();
	initShader();
	glutReshapeFunc(resizeWindow);
	glutIdleFunc(render);
	glutDisplayFunc(render);
	glutMainLoop();

	
	freeVBO();
}

