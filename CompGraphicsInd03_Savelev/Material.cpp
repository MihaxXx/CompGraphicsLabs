#include "Material.h"

Material new_material(glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, glm::vec4 emission, float shininess, glm::vec4 color)
{
	Material m;
	m.ambient = ambient;
	m.diffuse = diffuse;
	m.specular = specular;
	m.emission = emission;
	m.shininess = shininess;
	m.color = color;
	return m;
}

void set_uniform_material(GLShader & glShader, Material m)
{
	glShader.setUniform(glShader.getUniformLocation("material.ambient"), m.ambient);
	glShader.setUniform(glShader.getUniformLocation("material.diffuse"), m.diffuse);
	glShader.setUniform(glShader.getUniformLocation("material.specular"), m.specular);
	glShader.setUniform(glShader.getUniformLocation("material.emission"), m.emission);
	glShader.setUniform(glShader.getUniformLocation("material.shininess"), m.shininess);
	glShader.setUniform(glShader.getUniformLocation("material.color"), m.color);
}