#include "Material.h"

Material get_some_material()
{
	Material m;
	m.ambient = glm::vec4(0.2, 0.2, 0.2, 1.0);
	m.diffuse = glm::vec4(0.7, 0.7, 0.7, 1.0);
	m.specular = glm::vec4(0.4, 0.4, 0.4, 1.0);
	m.emission = glm::vec4(0.1, 0.1, 0.1, 1.0);
	m.shininess = 0.1 * 128;
	m.color_obj = glm::vec4(0.7, 0.0, 0.7, 1.0);
	return m;
}


void set_uniform_material(GLShader & glShader, Material m)
{
	glShader.setUniform(glShader.getUniformLocation("material.ambient"), m.ambient);
	glShader.setUniform(glShader.getUniformLocation("material.diffuse"), m.diffuse);
	glShader.setUniform(glShader.getUniformLocation("material.specular"), m.specular);
	glShader.setUniform(glShader.getUniformLocation("material.emission"), m.emission);
	glShader.setUniform(glShader.getUniformLocation("material.shininess"), m.shininess);
	glShader.setUniform(glShader.getUniformLocation("material.color_obj"), m.color_obj);
}