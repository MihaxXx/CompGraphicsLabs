#include <GL/glew.h>
#include <gl/GL.h>   // GL.h header file    
#include <gl/GLU.h> // GLU.h header file     
#include <gl/freeglut.h>
#include <gl/glaux.h>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtx/transform.hpp"
#include <iostream>
#include "GL/SOIL.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include "GLShader.h"
#include "Light.h"
#include "Material.h"

using namespace std;

GLShader glShader;

GLint Unif_matrix;

glm::mat4 Matrix_projection;

GLuint VBO, VAO, EBO;

GLuint VBO_position, VBO_texcoord, VBO_normal, VBO_element;

class Model
{
public:

	vector<glm::vec3> vertices_m = vector<glm::vec3>();
	vector<glm::vec2> tex_coords = vector<glm::vec2>();
	vector<glm::vec3> normals = vector<glm::vec3>();
	vector<GLint> indices = vector<GLint>();

	Model() {}

	Model(char* path)
	{
		loadModel(path);
	}

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
		string directory = path.substr(0, path.find_last_of('/'));

		processNode(scene->mRootNode, scene);
	}

	// Обработка узлов
	void processNode(aiNode* node, const aiScene* scene)
	{
		// Обрабатываем все меши (если они есть) у выбранного узла
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			processMesh(mesh, scene);
		}
		// И проделываем то же самое для всех дочерних узлов
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	void processMesh(aiMesh* mesh, const aiScene* scene)
	{
		// Цикл по всем вершинам меша
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			glm::vec3 vector; // объявляем промежуточный вектор, т.к. Assimp использует свой собственный векторный класс, который не преобразуется напрямую в тип glm::vec3, поэтому сначала мы передаем данные в этот промежуточный вектор типа glm::vec3

			// Координаты
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertices_m.push_back(vector);

			// Нормали
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			normals.push_back(vector);

			// Текстурные координаты
			if (mesh->mTextureCoords[0]) // если меш содержит текстурные координаты
			{
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				tex_coords.push_back(vec);
			}
			else
				tex_coords.push_back(glm::vec2(0.0f, 0.0f));
		}

		GLint last_ind = indices.size();
		// Теперь проходимся по каждой грани меша (грань - это треугольник меша) и извлекаем соответствующие индексы вершин
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// Получаем все индексы граней и сохраняем их в векторе indices
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j] + last_ind);
		}
	}
};

//! Проверка ошибок OpenGL, если есть то вывод в консоль тип ошибки 
void checkOpenGLerror(string str)
{
	GLenum errCode;
	if ((errCode = glGetError()) != GL_NO_ERROR)
		std::cout << "OpenGl error! - " << gluErrorString(errCode) << str << "\n";
}

//! Инициализация шейдеров 
void initShader()
{
	//glShader.loadFiles("shaders/vertex3.txt", "shaders/fragment3.txt");
	glShader.loadFiles("shaders/vertex31.txt", "shaders/fragment31.txt");
	checkOpenGLerror("initShader");
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


GLsizei indices_count = 0;

//! Инициализация VBO 
void initVBO()
{
	Model glModel = Model("untitled.obj");
	std::vector<glm::vec3> vert = glModel.vertices_m;
	std::vector<glm::vec2> tex_vert = glModel.tex_coords;
	std::vector<glm::vec3> norm_vert = glModel.normals;
	std::vector<GLint> indices = glModel.indices;

	indices_count = indices.size();

	glGenBuffers(1, &VBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
	glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(glm::vec3), &vert[0], GL_STATIC_DRAW);

	glGenBuffers(1, &VBO_texcoord);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_texcoord);
	glBufferData(GL_ARRAY_BUFFER, tex_vert.size() * sizeof(glm::vec2), &tex_vert[0], GL_STATIC_DRAW);

	glGenBuffers(1, &VBO_normal);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_normal);
	glBufferData(GL_ARRAY_BUFFER, norm_vert.size() * sizeof(glm::vec3), &norm_vert[0], GL_STATIC_DRAW);

	glGenBuffers(1, &VBO_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GL_UNSIGNED_INT), &indices[0], GL_STATIC_DRAW);

	checkOpenGLerror("initVBO");
}

//! Освобождение буфера
void freeVBO()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO_normal);
	glDeleteBuffers(1, &VBO_texcoord);
	glDeleteBuffers(1, &VBO_element);
	glDeleteBuffers(1, &VBO_position);
}
double angle_x = 0;

void resizeWindow(int width, int height)
{
	glViewport(0, 0, width, height);
}

/*
//! Отрисовка 
void render()
{
	angle_x += 0.0007;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 1000.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::mat4 rotate_y = { glm::cos(angle_x), 0.0f, glm::sin(angle_x), 0.0f,
					   0.0f, 1, 0, 0.0f,
					   -glm::sin(angle_x),0, glm::cos(angle_x), 0.0f,
					   0.0f, 0.0f, 0.0f, 1.0f };

	Matrix_projection = Projection * View * rotate_y;

	//! Устанавливаем шейдерную программу текущей 
	glShader.use();
	glShader.setUniform(glShader.getUniformLocation("matrix"), Matrix_projection);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_element);

	glEnableVertexAttribArray(glShader.getAttribLocation("texCoord"));
	glBindBuffer(GL_ARRAY_BUFFER, VBO_texcoord);
	glVertexAttribPointer(glShader.getAttribLocation("texCoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(glShader.getAttribLocation("position"));
	glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
	glVertexAttribPointer(glShader.getAttribLocation("position"), 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glShader.setUniform(glShader.getUniformLocation("ourTexture"), 0);


	glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, 0);

	for (int i = 0; i < 2; i++)
		glDisableVertexAttribArray(i);

	glFlush();

	checkOpenGLerror();
	glutSwapBuffers();
}*/

//! Отрисовка 
void render()
{
	angle_x += 0.0007;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 1000.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(5, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::mat4 rotate_y = { glm::cos(angle_x), 0.0f, glm::sin(angle_x), 0.0f,
					   0.0f, 1, 0, 0.0f,
					   -glm::sin(angle_x),0, glm::cos(angle_x), 0.0f,
					   0.0f, 0.0f, 0.0f, 1.0f };


	glm::mat4 Model = rotate_y;
	glm::mat4 ViewProjection = Projection * View;

	glm::mat3 normalMatrix = glm::transpose(glm::inverse(Model));

	//! Устанавливаем шейдерную программу текущей 
	glShader.use();
	checkOpenGLerror("render");
	glShader.setUniform(glShader.getUniformLocation("transform.model"), Model);
	glShader.setUniform(glShader.getUniformLocation("transform.viewProjection"), ViewProjection);
	glShader.setUniform(glShader.getUniformLocation("transform.normal"), normalMatrix);
	glShader.setUniform(glShader.getUniformLocation("transform.viewPosition"), vec3(4, 3, 3));

	set_uniform_point_light(glShader, get_some_point_light());
	set_uniform_material(glShader, get_some_material());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_element);

	glEnableVertexAttribArray(glShader.getAttribLocation("position"));
	glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
	glVertexAttribPointer(glShader.getAttribLocation("position"), 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(glShader.getAttribLocation("texcoord"));
	glBindBuffer(GL_ARRAY_BUFFER, VBO_texcoord);
	glVertexAttribPointer(glShader.getAttribLocation("texcoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(glShader.getAttribLocation("normal"));
	glBindBuffer(GL_ARRAY_BUFFER, VBO_normal);
	glVertexAttribPointer(glShader.getAttribLocation("normal"), 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glShader.setUniform(glShader.getUniformLocation("ourTexture"), 0);


	glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, 0);

	for (int i = 0; i < 2; i++)
		glDisableVertexAttribArray(i);

	glFlush();


	glutSwapBuffers();
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

	//! Освобождение ресурсов  
	freeVBO();
}