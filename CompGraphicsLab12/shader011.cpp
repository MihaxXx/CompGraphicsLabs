#include "GL\glew.h" 
#include "GL\freeglut.h"
#include <iostream>
//! ���������� � ����������������� ID 
//! //! ID ��������� ���������
GLuint Program;
//! ID ��������
GLint  Attrib_vertex;
//! ID ������� ���������� ����� 
GLint Unif_color;
//! �������� ������ OpenGL, ���� ���� �� ����� � ������� ��� ������ 
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
		"attribute vec2 coord;\n"
		"mat2 rot(in float a) {return mat2(cos(a), sin(a), -sin(a), cos(a));}\n"
		"void main() {\n"
		"vec2 pos = rot(3.14*0.25)*coord;\n"
		"  gl_Position = vec4(pos, 0.0, 1.0);\n"
		"}\n";
	const char* fsSource =
		"uniform vec4 color;\n"
		"void main() {\n"
		"  gl_FragColor = color;\n"
		"}\n";
	//! ���������� ��� �������� ��������������� �������� 
	GLuint vShader, fShader;
	//! ������� ��������� ������
	vShader = glCreateShader(GL_VERTEX_SHADER);
	//! �������� �������� ��� 
	glShaderSource(vShader, 1, &vsSource, NULL);
	//! ����������� ������ 
	glCompileShader(vShader);
	//! ������� ����������� ������
	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	//! �������� �������� ��� 
	glShaderSource(fShader, 1, &fsSource, NULL);
	//! ����������� ������ 
	glCompileShader(fShader);
	//! ������� ��������� � ����������� ������� � ��� 
	Program = glCreateProgram();
	glAttachShader(Program, vShader);
	glAttachShader(Program, fShader);
	//! ������� ��������� ��������� 
	glLinkProgram(Program);
	//! ��������� ������ ������
	int link_ok;
	glGetProgramiv(Program, GL_LINK_STATUS, &link_ok); if (!link_ok)
	{
		std::cout << "error attach shaders \n";
		return;
	}

	///! ���������� ID �������� �� ��������� ���������
	const char* attr_name = "coord";
	Attrib_vertex = glGetAttribLocation(Program, attr_name);
	if (Attrib_vertex == -1)
	{
		std::cout << "could not bind attrib " << attr_name << std::endl; return;
	}

	//! ���������� ID �������
	const char* unif_name = "color";
	Unif_color = glGetUniformLocation(Program, unif_name); if (Unif_color == -1)
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
void resizeWindow(int width, int height) {
	glViewport(0, 0, width, height);
}
//! ��������� 
void render1() {
	glClear(GL_COLOR_BUFFER_BIT);
	//! ������������� ��������� ��������� ������� 
	glUseProgram(Program);
	static float red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	//! �������� ������� � ������ 
	glUniform4fv(Unif_color, 1, red);
	glBegin(GL_QUADS);
	glVertex2f(-0.5f, -0.5f);
	glVertex2f(-0.5f, 0.5f);
	glVertex2f(0.5f, 0.5f);
	glVertex2f(0.5f, -0.5f);
	glEnd();
	glFlush();
	//! ��������� ��������� ��������� 
	glUseProgram(0);
	checkOpenGLerror();
	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Simple shaders");
	glClearColor(0, 0, 1, 0);
	//! ����������� ����� �������������� �������� 
	GLenum glew_status = glewInit();
	if (GLEW_OK != glew_status)
	{
		//! GLEW �� ���������������������
		std::cout << "Error: " << glewGetErrorString(glew_status) << "\n"; return 1;
	}
	//! ��������� ����������� OpenGL 2.0 
	if (!GLEW_VERSION_2_0)
	{
		//! OpenGl 2.0 ��������� �� ��������
		std::cout << "No support for OpenGL 2.0 found\n"; return 1;
	}
	//! ������������� �������� 
	initShader();
	glutReshapeFunc(resizeWindow);
	glutDisplayFunc(render1);
	glutMainLoop();
	//! ������������ �������� 
	freeShader();
}