#include <Windows.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

//! ���������� � ����������������� ID

//! ID ��������� ���������
GLuint Program;

//! ID ���������
GLint  Attrib_vertex;

GLint Unif_matrix;

glm::mat4 Matrix_projection;

double rotate_z = 0;
float angle = 0;


GLfloat vertices[] =
{
	-1.0f , -1.0f , -1.0f ,
	1.0f , -1.0f , -1.0f ,
	1.0f , 1.0f , -1.0f ,
	-1.0f , 1.0f , -1.0f ,
	-1.0f , -1.0f , 1.0f ,
	1.0f , -1.0f , 1.0f ,
	1.0f , 1.0f , 1.0f ,
	-1.0f , 1.0f , 1.0f
};

GLfloat colors[] = {
  1.0f , 0.5f , 1.0f ,
  1.0f , 0.5f , 0.5f ,
  0.5f , 0.5f , 1.0f ,
  0.0f , 1.0f , 1.0f ,
  1.0f , 0.0f , 1.0f ,
  1.0f , 1.0f , 0.0f ,
  1.0f , 0.0f , 1.0f ,
  0.0f , 1.0f , 1.0f
};
int indices[] = {
	0, 4, 5,
	0, 5, 1,
	1, 5, 6,
	1, 6, 2,
	2, 6, 7,
	2, 7, 3,
	3, 7, 4,
	3, 4, 0,
	4, 7, 6,
	4, 6, 5,
	3, 0, 1,
	3, 1, 2
};
auto Indices_count = sizeof(indices) / sizeof(indices[0]);


//! �������� ������ OpenGL, ���� ����, �� ����� � ������� ��� ������
void checkOpenGLerror() {
	GLenum errCode;
	if ((errCode = glGetError()) != GL_NO_ERROR)
		std::cout << "OpenGl error! - " << gluErrorString(errCode) << "\n";
}

//! ������������� ��������
void initShader()
{
	//! �������� ��� �������� 
	const char* vsSource =
		"attribute vec3 coord;\n"
		"varying vec4 var_color;\n"
		"uniform mat4 matrix;\n"
		"void main() {\n"
		"	gl_Position = matrix*vec4(coord, 1.0);\n"
		"	var_color = gl_Color;\n"
		"}\n";
	const char* fsSource =
		"varying vec4 var_color;\n"
		"void main() {\n"
		"  gl_FragColor = var_color;\n"
		"}\n";
	//! ���������� ��� �������� ��������������� �������� 
	GLuint vShader, fShader;
	//! ������� ��������� ������
	vShader = glCreateShader(GL_VERTEX_SHADER);
	//! �������� �������� ��� 
	glShaderSource(vShader, 1, &vsSource, NULL);
	//! ����������� ������ 
	glCompileShader(vShader);
	int compile_ok;
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &compile_ok);
	if (!compile_ok)
	{
		std::cout << "error compile shaders \n";
		int loglen;
		glGetShaderiv(vShader, GL_INFO_LOG_LENGTH, &loglen);
		int realLogLen;
		char* log = new char[loglen];
		glGetShaderInfoLog(vShader, loglen, &realLogLen, log);
		std::cout << log << "\n";
		delete log;
	}

	//! ������� ����������� ������
	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	//! �������� �������� ��� 
	glShaderSource(fShader, 1, &fsSource, NULL);
	//! ����������� ������ 
	glCompileShader(fShader);
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &compile_ok);
	if (!compile_ok)
	{
		std::cout << "error compile shaders \n";
		int loglen;
		glGetShaderiv(fShader, GL_INFO_LOG_LENGTH, &loglen);
		int realLogLen;
		char* log = new char[loglen];
		glGetShaderInfoLog(fShader, loglen, &realLogLen, log);
		std::cout << log << "\n";
		delete log;
	}

	//! ������� ��������� � ����������� ������� � ��� 
	Program = glCreateProgram();
	glAttachShader(Program, vShader);
	glAttachShader(Program, fShader);
	//! ������� ��������� ��������� 
	glLinkProgram(Program);
	//! ��������� ������ ������
	int link_ok;
	glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
	if (!link_ok)
	{
		std::cout << "error attach shaders \n";
		int loglen;
		glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &loglen);
		int realLogLen;
		char* log = new char[loglen];
		glGetProgramInfoLog(Program, loglen, &realLogLen, log);
		std::cout << log << "\n";
		delete log;
	}

	///! ���������� ID �������� �� ��������� ���������
	const char* attr_name = "coord";
	Attrib_vertex = glGetAttribLocation(Program, attr_name);
	if (Attrib_vertex == -1)
	{
		std::cout << "could not bind attrib " << attr_name << std::endl; return;
	}

	const char* unif_name = "matrix";
	Unif_matrix = glGetUniformLocation(Program, unif_name);
	if (Unif_matrix == -1)
	{
		std::cout << "could not bind uniform " << unif_name << std::endl; return;
	}
	checkOpenGLerror();
}

//! ������������ ��������
void freeShader()
{
	//! ��������� ����, �� ��������� �������� ���������
	glUseProgram(0);
	//! ������� ��������� ���������
	glDeleteProgram(Program);
}

void resizeWindow(int width, int height)
{
	glViewport(0, 0, width, height);
}

//! ���������
void render2()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	angle += 0.0005f;
	glLoadIdentity();

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::mat4 rotate_x = { 1.0f, 0.0f, 0.0f, 0.0f,
						   0.0f, glm::cos(angle), -glm::sin(angle), 0.0f,
						   0.0f, glm::sin(angle), glm::cos(angle), 0.0f,
						   0.0f, 0.0f, 0.0f, 1.0f };


	Matrix_projection = Projection * View/* * Model */ * rotate_x;
	//! ������������� ��������� ��������� �������
	glUseProgram(Program);
	//! �������� ������� � ������
	glUniformMatrix4fv(Unif_matrix, 1, GL_FALSE, &Matrix_projection[0][0]);

	glBegin(GL_TRIANGLES);
	for (int i = 0; i < Indices_count; i++)
	{
		glColor3f(colors[indices[i] * 3], colors[indices[i] * 3 + 1], colors[indices[i] * 3 + 2]);
		glVertex3f(vertices[indices[i] * 3], vertices[indices[i] * 3 + 1], vertices[indices[i] * 3 + 2]);
	}
	glEnd();
	//glFlush();
	//! ��������� ��������� ���������
	glUseProgram(0);
	checkOpenGLerror();
	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "Russian");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Simple shaders");
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearColor(0, 0, 1, 0);
	//! ����������� ����� �������������� ��������
	GLenum glew_status = glewInit();
	if (GLEW_OK != glew_status)
	{
		//! GLEW �� ���������������������
		std::cout << "Error: " << glewGetErrorString(glew_status) << "\n";
		return 1;
	}
	//! ��������� ����������� OpenGL 2.0
	if (!GLEW_VERSION_2_0)
	{
		//! OpenGl 2.0 ��������� �� ��������
		std::cout << "No support for OpenGL 2.0 found\n";
		return 1;
	}
	//! ������������� ��������
	initShader();
	glutReshapeFunc(resizeWindow);
	glutIdleFunc(render2);
	glutDisplayFunc(render2);
	glutMainLoop();
	//! ������������ ��������
	freeShader();
}