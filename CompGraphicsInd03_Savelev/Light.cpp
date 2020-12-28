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

void set_uniform_point_light(GLShader& glShader, PointLight l, string lightName)
{
	glShader.setUniform(glShader.getUniformLocation(lightName+".position"), l.position);
	glShader.setUniform(glShader.getUniformLocation(lightName+".ambient"), l.ambient);
	glShader.setUniform(glShader.getUniformLocation(lightName+".diffuse"), l.diffuse);
	glShader.setUniform(glShader.getUniformLocation(lightName+".specular"), l.specular);
	glShader.setUniform(glShader.getUniformLocation(lightName+".attenuation"), l.attenuation);
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

Spotlight new_spotlight(glm::vec4 position, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, glm::vec3 attenuation,
	glm::vec3 direction, float angle)
{
	Spotlight l;
	l.position = position;
	l.ambient = ambient;
	l.diffuse = diffuse;
	l.specular = specular;
	l.attenuation = attenuation;
	l.direction = direction;
	l.angle = angle;
	return l;
}

Spotlight new_spotlight()
{
	Spotlight l;
	l.position = glm::vec4(1.0, 1.0, 1.0, 1.0);
	l.ambient = glm::vec4(0.7, 0.7, 0.7, 1.0);
	l.diffuse = glm::vec4(0.7, 0.7, 0.7, 1.0);
	l.specular = glm::vec4(1.0, 1.0, 1.0, 1.0);
	l.attenuation = glm::vec3(0.7, 0.7, 0.7);
	l.direction = glm::vec3(0.0, 0.0, 1.0);
	l.angle = 60/360;
	return l;
}

void set_uniform_spotlight(GLShader& glShader, Spotlight l)
{
	glShader.setUniform(glShader.getUniformLocation("light2.position"), l.position);
	glShader.setUniform(glShader.getUniformLocation("light2.ambient"), l.ambient);
	glShader.setUniform(glShader.getUniformLocation("light2.diffuse"), l.diffuse);
	glShader.setUniform(glShader.getUniformLocation("light2.specular"), l.specular);
	glShader.setUniform(glShader.getUniformLocation("light2.attenuation"), l.attenuation);
	glShader.setUniform(glShader.getUniformLocation("light2.direction"), l.direction);
	glShader.setUniform(glShader.getUniformLocation("light2.angle"), l.angle);
}