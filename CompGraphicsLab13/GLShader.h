#pragma once
#include "GL\glew.h"
#include "GL\freeglut.h"
#include <iostream>
#include <fstream>
#include <string>

#include "glm\glm.hpp"

using namespace glm;
using namespace std;

class GLShader
{
private:
	GLuint ShaderProgram;
	GLuint vertex_shader;
	GLuint fragment_shader;

private:
	void printInfoLogShader(GLuint shader);
	void printInfoLogProgram(GLuint shader);
	//GLuint loadSourcefile(const string& source_file_name,
		//GLuint shader_type);
	GLuint compileSource(const GLchar* source,
		GLuint shader_type);
	void linkProgram();

public:
	GLShader() : ShaderProgram(0) {}
	~GLShader();

	GLuint loadFiles(const string& vertex_file_name, const string& fragment_file_name);
	GLuint load(const string& vertex_source, const string& fragment_source);
	GLuint load(const GLchar* vertex_source, const GLchar* fragment_source);

	void use() { glUseProgram(ShaderProgram); }

	GLuint getIDProgram() { return ShaderProgram; }
	bool isLoad() { return ShaderProgram != 0; }

	// Attribute
	GLint getAttribLocation(const GLchar* name) const;
	GLint getAttribLocation(const string& name) const;

	// Uniform get
	GLint getUniformLocation(const GLchar* name) const;
	GLint getUniformLocation(const std::string& name) const;

	// Uniform set 
	void setUniform(GLint location, const vec4& value);
	void setUniform(GLint location, const vec3& value);
	void setUniform(GLint location, const vec2& value);
	void setUniform(GLint location, const mat4& value);
	void setUniform(GLint location, const GLint value);
	void setUniform(GLint location, const GLuint value);
	void setUniform(GLint location, const GLfloat value);
	void setUniform(GLint location, const float* value);
};

