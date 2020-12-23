#ifndef MATERIAL
#define MATERIAL
#include "glm/glm.hpp"
#include "GLShader.h"

struct Material
{
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec4 emission;
	float shininess;
	glm::vec4 color;
};

Material new_material(glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, glm::vec4 emission, float shininess, glm::vec4 color);

void set_uniform_material(GLShader & glShader, Material m);

#endif

