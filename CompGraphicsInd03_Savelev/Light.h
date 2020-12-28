#ifndef LIGHT
#define LIGHT
#include "glm/glm.hpp"
#include "GLShader.h"

struct PointLight
{
	glm::vec4 position;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec3 attenuation;
};

PointLight new_point_light(glm::vec4 position, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, glm::vec3 attenuation);

void set_uniform_point_light(GLShader & glShader, PointLight l, string lightName = "light");



struct DirectLight
{
	glm::vec4 direction;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
};

DirectLight get_some_direction_light();
DirectLight new_direction_light(glm::vec4 direction, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular);
void set_uniform_direct_light(GLShader& glShader, DirectLight l);

struct Spotlight
{
	glm::vec4 position;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec3 attenuation;
	glm::vec3 direction;
	float angle;
};

Spotlight new_spotlight();
Spotlight new_spotlight(glm::vec4 position, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, glm::vec3 attenuation,
	glm::vec3 direction, float angle);
void set_uniform_spotlight(GLShader& glShader, Spotlight l);

#endif

