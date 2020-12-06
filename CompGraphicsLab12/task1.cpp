#include <iostream>
#include <Windows.h>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <GL/freeglut.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include <GL/freeglut_std.h>

//! ���������� � ����������������� ID

//! ID ��������� ���������
GLuint Program;

//! ID ������� ���������� �����
GLint Unif_color;

//! ID ������� ���������� �����
GLint Unif_color2;

double rotate_z = 0;
float angle = 0;

//! �������� ������ OpenGL, ���� ����, �� ����� � ������� ��� ������
void checkOpenGLerror() {
	GLenum errCode;
	if ((errCode = glGetError()) != GL_NO_ERROR)
		std::cout << "OpenGl error! - " << gluErrorString(errCode);
}

//! ������������� ��������
void initShader()
{
	//! �������� ��� ��������
	const char* vsSource =
		"attribute vec2 coord;\n"
		"void main() {\n"
		" gl_Position = vec4(coord, 0.0, 1.0);\n"
		"}\n";
	const char* fsSource =
		"uniform vec4 color;\n"
		"uniform vec4 color2;\n"
		"void main() {\n"
		"vec2 st = gl_PointCoord;\n"
		"float mixValue = distance(st, vec2(0.0, 1.0));\n"
		"vec3 color1 = mix(color, color2, mixValue);\n"
		"gl_FragColor = vec4(color1, 1.0);\n"
		"}\n";

	//! ���������� ��� �������� ��������������� ��������
	GLuint fShader;
	//! ������� ����������� ������
	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	//! �������� �������� ���
	glShaderSource(fShader, 1, &fsSource, NULL);
	//! ����������� ������
	glCompileShader(fShader);
	//! ������� ��������� � ����������� ������� � ���
	Program = glCreateProgram();
	glAttachShader(Program, fShader);
	//! ������� ��������� ���������
	glLinkProgram(Program);
	//! ��������� ������ ������
	int link_ok;
	glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
	if (!link_ok)
	{
		std::cout << "error attach shaders \n";
		return;
	}
	//! ���������� ID �������
	const char* unif_name = "color";
	Unif_color = glGetUniformLocation(Program, unif_name);
	if (Unif_color == -1)
	{
		std::cout << "could not bind uniform " << unif_name << std::endl;
		return;
	}

	//! ���������� ID �������
	const char* unif_name2 = "color2";
	Unif_color2 = glGetUniformLocation(Program, unif_name2);
	if (Unif_color2 == -1)
	{
		std::cout << "could not bind uniform " << unif_name2 << std::endl;
		return;
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

	angle += 0.05f;
	glLoadIdentity();
	glTranslatef(0.5, 0.5, 0.0);
	glScalef(0.5, 0.5, 0.5);
	glRotatef(angle, 0.0, 1.0, 0.0);
	glTranslatef(-0.5, -0.5, 0.0);
	//! ������������� ��������� ��������� �������
	glUseProgram(Program);
	static float red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	static float green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	static float blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	//! �������� ������� � ������
	glUniform4fv(Unif_color, 1, green);
	glUniform4fv(Unif_color2, 1, red);
	glBegin(GL_TRIANGLES);
	glColor3f(1.0, 0.0, 0.0); glVertex2f(-0.5f, -0.5f);
	glColor3f(0.0, 1.0, 0.0); glVertex2f(-0.5f, 0.5f);
	glColor3f(1.0, 1.0, 1.0); glVertex2f(0.5f, -0.5f);
	glEnd();
	glFlush();
	//! ��������� ��������� ���������
	glUseProgram(0);
	checkOpenGLerror();
	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	//setlocale(LC_ALL, "Russian");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Simple shaders");
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