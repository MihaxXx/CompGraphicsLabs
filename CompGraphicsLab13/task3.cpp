#include <GL/glew.h>
#include <gl/GL.h>   // GL.h header file    
#include <gl/GLU.h> // GLU.h header file     
#include <gl/freeglut.h>
#include <gl/glaux.h>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "GL/SOIL.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

using namespace std;

GLint Program;

GLint Unif_matrix;

glm::mat4 Matrix_projection;

GLuint VBO, VAO, EBO;

struct Vertex 
{
	glm::vec3 Position; // Позиция
	glm::vec3 Normal; // Нормаль
	glm::vec2 TexCoords; // Текстурные координаты
};

struct Texture 
{
	unsigned int id;
	string type;
	string path;
};

class Mesh 
{
public:
	// Данные меша
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	//unsigned int VAO;

	// Конструктор
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices)
	{
		this->vertices = vertices;
		this->indices = indices;

		// Теперь, когда у нас есть все необходимые данные, устанавливаем вершинные буферы и указатели атрибутов
		setupMesh();
	}

private:
	// Данные для рендеринга 
	//unsigned int VBO, EBO;

	// Инициализируем все буферные объекты/массивы
	void setupMesh()
	{
		// Создаем буферные объекты/массивы
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		// Загружаем данные в вершинный буфер
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// Самое замечательное в структурах то, что расположение в памяти их внутренних переменных является последовательным.
		// Смысл данного трюка в том, что мы можем просто передать указатель на структуру, и она прекрасно преобразуется в массив данных с элементами типа glm::vec3 (или glm::vec2), который затем будет преобразован в массив данных float, ну а в конце – в байтовый массив
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// Устанавливаем указатели вершинных атрибутов

		// Координаты вершин
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		// Нормали вершин
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		// Текстурные координаты вершин
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);
	}
};
class Model
{
public:
	Model() {}

	Model(char* path)
	{
		loadModel(path);
	}

	// Данные модели
	vector<Mesh> meshes;
	string directory;

	// Загрузка модели
	void loadModel(string path)
	{
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
			return;
		}
		directory = path.substr(0, path.find_last_of('/'));

		processNode(scene->mRootNode, scene);
	}

	// Обработка узлов
	void processNode(aiNode* node, const aiScene* scene)
	{
		// Обрабатываем все меши (если они есть) у выбранного узла
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		// И проделываем то же самое для всех дочерних узлов
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		// Данные для заполнения
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<Texture> textures;

		// Цикл по всем вершинам меша
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector; // объявляем промежуточный вектор, т.к. Assimp использует свой собственный векторный класс, который не преобразуется напрямую в тип glm::vec3, поэтому сначала мы передаем данные в этот промежуточный вектор типа glm::vec3

			// Координаты
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;

			// Нормали
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;

			// Текстурные координаты
			if (mesh->mTextureCoords[0]) // если меш содержит текстурные координаты
			{
				glm::vec2 vec;
				// Вершина может содержать до 8 различных текстурных координат. Мы предполагаем, что мы не будем использовать модели,
				// в которых вершина может содержать несколько текстурных координат, поэтому мы всегда берем первый набор (0)
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			vertices.push_back(vertex);
		}

		// Теперь проходимся по каждой грани меша (грань - это треугольник меша) и извлекаем соответствующие индексы вершин
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// Получаем все индексы граней и сохраняем их в векторе indices
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		// Возвращаем mesh-объект, созданный на основе полученных данных
		return Mesh(vertices, indices);
	}
};

Model mod;

//! Вершина 
struct vertex
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

//! Функция печати лога шейдера 
void shaderLog(unsigned int shader)
{
	int   infologLen = 0;
	int   charsWritten = 0;
	char* infoLog;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
	if (infologLen > 1)
	{
		infoLog = new char[infologLen];
		if (infoLog == NULL)
		{
			std::cout << "ERROR: Could not allocate InfoLog buffer\n";
			exit(1);
		}
		glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog);
		std::cout << "InfoLog: " << infoLog << "\n\n\n";
		delete[] infoLog;
	}
}


//! Проверка ошибок OpenGL, если есть то вывод в консоль тип ошибки 
void checkOpenGLerror()
{
	GLenum errCode;
	if ((errCode = glGetError()) != GL_NO_ERROR)
		std::cout << "OpenGl error! - " << gluErrorString(errCode);
}

//! Инициализация шейдеров 
void initShader()
{
	//! Исходный код шейдеров  	
	const char* vsSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 position;\n"
		"layout(location = 1) in vec3 color;\n"
		"layout(location = 2) in vec2 texCoord;\n"
		"uniform mat4 matrix;\n"
		"out vec3 ourColor;\n" // выходной параметр — собственный цвет
		"out vec2 TexCoord;\n" // выходной параметр — текстурный цвет
		"void main()\n"
		"{\n"
		"gl_Position = matrix * vec4(position, 0.5f);\n"
		"ourColor = color;\n"
		"TexCoord = texCoord;\n"
		"}\n";
	const char* fsSource =
		"in vec3 ourColor;\n"
		"in vec2 TexCoord;\n"
		"out vec4 color;\n"
		"uniform sampler2D ourTexture;\n"
		"void main() {\n"
		"color = texture(ourTexture, TexCoord);\n" 
		"}\n";
	//! Переменные для хранения идентификаторов шейдеров 
	GLuint vShader, fShader;
	//! Создаем вершинный шейдер 
	vShader = glCreateShader(GL_VERTEX_SHADER);
	//! Передаем исходный код  
	glShaderSource(vShader, 1, &vsSource, NULL);

	//! Компилируем шейдер  	
	glCompileShader(vShader);

	std::cout << "vertex shader \n";
	shaderLog(vShader);

	//! Создаем фрагментный шейдер 
	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	//! Передаем исходный код 
	glShaderSource(fShader, 1, &fsSource, NULL);
	//! Компилируем шейдер  	
	glCompileShader(fShader);
	std::cout << "fragment shader \n";  	shaderLog(fShader);

	//! Создаем программу и прикрепляем шейдеры к ней 
	Program = glCreateProgram();  	glAttachShader(Program, vShader);  	glAttachShader(Program, fShader);

	//! Линкуем шейдерную программу  	
	glLinkProgram(Program);

	//! Проверяем статус сборки 
	int link_ok;
	glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);  	if (!link_ok)
	{
		std::cout << "error attach shaders \n";
		return;
	}

	const char* attr_name = "matrix";
	Unif_matrix = glGetUniformLocation(Program, attr_name);

	checkOpenGLerror();
}

// Load and create a texture 
GLuint texture;
void text()
{
	// Текстура 2
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
}


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

	/*glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);*/

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	checkOpenGLerror();
}

//! Освобождение шейдеров 
void freeShader()
{
	//! Передавая ноль, мы отключаем шейдрную программу 
	glUseProgram(0);
	//! Удаляем шейдерную программу  
	glDeleteProgram(Program);
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

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 1000.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(40, 40, 40), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::mat4 rotate_y = { glm::cos(angle_x), 0.0f, glm::sin(angle_x), 0.0f,
					   0.0f, 1, 0, 0.0f,
					   -glm::sin(angle_x),0, glm::cos(angle_x), 0.0f,
					   0.0f, 0.0f, 0.0f, 1.0f };

	Matrix_projection = Projection * View * rotate_y;

	//! Устанавливаем шейдерную программу текущей 
	glUseProgram(Program);
	glUniformMatrix4fv(Unif_matrix, 1, GL_FALSE, &Matrix_projection[0][0]);
	/*
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO*/

	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(Program, "ourTexture"), 0);

	GLsizei count = 0;
	for (auto mesh : mod.meshes)
		count += mesh.indices.size();

	// Draw container
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glFlush();

	checkOpenGLerror();

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
	mod = Model("medieval house.obj");
	//initVBO();
	initShader();
	glutReshapeFunc(resizeWindow);
	glutIdleFunc(render);
	glutDisplayFunc(render);
	glutMainLoop();

	//! Освобождение ресурсов  
	freeShader();
	freeVBO();
}