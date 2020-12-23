#include "Light.h"

PointLight new_point_light(glm::vec4 position, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, glm::vec3 attenuation)
{
	PointLight l;
	l.position = position;
	l.ambient = ambient;
	l.diffuse = diffuse;
	l.specular = specular;
	l.attenuation = attenuation;
	return l;
}

void set_uniform_point_light(GLShader& glShader, PointLight l)
{
	glShader.setUniform(glShader.getUniformLocation("light.position"), l.position);
	glShader.setUniform(glShader.getUniformLocation("light.ambient"), l.ambient);
	glShader.setUniform(glShader.getUniformLocation("light.diffuse"), l.diffuse);
	glShader.setUniform(glShader.getUniformLocation("light.specular"), l.specular);
	glShader.setUniform(glShader.getUniformLocation("light.attenuation"), l.attenuation);
}

DirectLight new_direction_light(glm::vec4 direction, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular)
{
	DirectLight l;
	l.direction = direction;
	l.ambient = ambient;
	l.diffuse = diffuse;
	l.specular = specular;
	return l;
}

DirectLight get_some_direction_light()
{
	DirectLight l;
	l.direction = glm::vec4(-1.0, 0.0, 0.0, 0.0);
	l.ambient = glm::vec4(0.5, 0.5, 0.5, 1.0);
	l.diffuse = glm::vec4(0.7, 0.7, 0.7, 1.0);
	l.specular = glm::vec4(1.0, 1.0, 1.0, 1.0);
	return l;
}


void set_uniform_direct_light(GLShader& glShader, DirectLight l)
{
	glShader.setUniform(glShader.getUniformLocation("dirlight.direction"), l.direction);
	glShader.setUniform(glShader.getUniformLocation("dirlight.ambient"), l.ambient);
	glShader.setUniform(glShader.getUniformLocation("dirlight.diffuse"), l.diffuse);
	glShader.setUniform(glShader.getUniformLocation("dirlight.specular"), l.specular);
}

Spotlight get_some_spotlight(glm::vec4 position, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, glm::vec3 attenuation,
							 glm::vec4 spotdirection, float spotcutoff, float spotexponent)
{
	Spotlight l;
	l.position = position;
	l.ambient = ambient;
	l.diffuse = diffuse;
	l.specular = specular;
	l.attenuation = attenuation;
	l.spotdirection = spotdirection;
	l.spotcutoff = spotcutoff;
	l.spotexponent = spotexponent;
	return l;
}

Spotlight get_some_spotlight()
{
	Spotlight l;
	l.position = glm::vec4(1.0, 1.0, 1.0, 1.0);
	l.ambient = glm::vec4(0.7, 0.7, 0.7, 1.0);
	l.diffuse = glm::vec4(0.7, 0.7, 0.7, 1.0);
	l.specular = glm::vec4(1.0, 1.0, 1.0, 1.0);
	l.attenuation = glm::vec3(0.7, 0.7, 0.7);
	l.spotdirection = glm::vec4(0.0, 0.0, -1.0, 0.0);
	l.spotcutoff = 60;
	l.spotexponent = 128;
	return l;
}

void set_uniform_direct_light(GLShader& glShader, Spotlight l)
{
	glShader.setUniform(glShader.getUniformLocation("spot.position"), l.position);
	glShader.setUniform(glShader.getUniformLocation("spot.ambient"), l.ambient);
	glShader.setUniform(glShader.getUniformLocation("spot.diffuse"), l.diffuse);
	glShader.setUniform(glShader.getUniformLocation("spot.specular"), l.specular);
	glShader.setUniform(glShader.getUniformLocation("spot.attenuation"), l.attenuation);
	glShader.setUniform(glShader.getUniformLocation("spot.spotdirection"), l.spotdirection);
	glShader.setUniform(glShader.getUniformLocation("spot.spotcutoff"), l.spotcutoff);
	glShader.setUniform(glShader.getUniformLocation("spot.spotexponent"), l.spotexponent);
}