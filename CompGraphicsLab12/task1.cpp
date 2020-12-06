#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>

//! ���������� � ����������������� ID
//! ID ��������� ���������
GLuint Program;
//! ID ���������
GLint  Attrib_vertex;
//! ID ������� ���������� �����
GLint Unif_angle;

float angle = 0;

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
		"#define PI 3.1415926538\n"
		"attribute vec2 coord;\n"
		"uniform float angle;\n"
		"varying vec4 var_color;\n"
		"mat2 rot(in float a) {return mat2(cos(a), sin(a), -sin(a), cos(a));}\n"
		"void main() {\n"
		"	vec2 pos = rot(PI*angle)*coord;\n"
		"	gl_Position = vec4(pos, 0.0, 1.0);\n"
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


	//! ���������� ID �������
	const char* unif_name = "angle";
	Unif_angle = glGetUniformLocation(Program, unif_name);
	if (Unif_angle == -1)
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

void specialKeys(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP: angle += 0.1; break;
	case GLUT_KEY_DOWN: angle -= 0.1; break;
	}
	glutPostRedisplay();
}


//! ���������
void render2()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	//! ������������� ��������� ��������� �������
	glUseProgram(Program);
	//! �������� ������� � ������
	glUniform1f(Unif_angle, angle);

	glBegin(GL_TRIANGLES);
	glColor3f(1.0, 0.0, 0.0);
	glVertex2f(-0.5f, -0.5f);
	glColor3f(0.0, 1.0, 0.0);
	glVertex2f(-0.5f, 0.5f);
	glColor3f(1.0, 1.0, 1.0);
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
	setlocale(LC_ALL, "Russian");
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
	glutDisplayFunc(render2);
	glutSpecialFunc(specialKeys);
	glutMainLoop();
	//! ������������ ��������
	freeShader();
}