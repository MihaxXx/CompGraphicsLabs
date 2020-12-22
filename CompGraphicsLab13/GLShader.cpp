#include "GLShader.h"

GLuint GLShader::compileSource(const GLchar* source,
	GLuint shader_type) {
	GLuint shader = glCreateShader(shader_type);
	//setting source for our shader code (we have only one so the array of the lenghts is NULL)
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

#ifdef _DEBUG
	printInfoLogShader(shader);
#endif
	return shader;
}
void GLShader::printInfoLogShader(GLuint shader) {
	int infologLen = 0;
	int charsWritten = 0;
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
void GLShader::printInfoLogProgram(GLuint shader) {

}

void GLShader::linkProgram() {
	ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, vertex_shader);
	glAttachShader(ShaderProgram, fragment_shader);
	glLinkProgram(ShaderProgram);

	int link_ok;
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &link_ok);
	if (!link_ok)
	{
		std::cout << "error attach shaders \n";
		ShaderProgram = GL_FALSE;
		return;
	}
}

GLuint GLShader::load(const string& vertex_source, const string& fragment_source) {
	vertex_shader = compileSource(vertex_source.c_str(),
		GL_VERTEX_SHADER);
	fragment_shader = compileSource(fragment_source.c_str(),
		GL_FRAGMENT_SHADER);
	linkProgram();
	return ShaderProgram;
}
GLuint GLShader::load(const GLchar* vertex_source, const GLchar* fragment_source) {
	vertex_shader = compileSource(vertex_source,
		GL_VERTEX_SHADER);
	fragment_shader = compileSource(fragment_source,
		GL_FRAGMENT_SHADER);
	linkProgram();
	return ShaderProgram;
}

GLuint GLShader::loadFiles(const string& vertex_file_name, const string& fragment_file_name)
{
	string s;
	string vSourse = "";
	ifstream f(vertex_file_name);
	while (!f.eof()) {
		f >> s;
		vSourse += s + "\n";
	}
	f.close();
	string fSourse = "";
	ifstream f1(fragment_file_name);
	while (!f1.eof()) {
		f1 >> s;
		fSourse += s + "\n";
	}
	f1.close();
	return load(vSourse, fSourse);
}

GLShader::~GLShader() {
	glUseProgram(0);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glDeleteProgram(ShaderProgram);
}

GLint GLShader::getAttribLocation(const GLchar* name) const {
	return glGetAttribLocation(ShaderProgram, name);
}
GLint GLShader::getAttribLocation(const string& name) const {
	return glGetAttribLocation(ShaderProgram, name.c_str());
}

GLint GLShader::getUniformLocation(const GLchar* name) const {
	return glGetUniformLocation(ShaderProgram, name);
}
GLint GLShader::getUniformLocation(const std::string& name) const {
	return glGetUniformLocation(ShaderProgram, name.c_str());
}
void GLShader::setUniform(GLint location, const vec4& value){
	glUniform4fv(location, 1, &value[0]);
}
void GLShader::setUniform(GLint location, const vec3& value){
	glUniform3fv(location, 1, &value[0]);
}
void GLShader::setUniform(GLint location, const vec2& value){
	glUniform2fv(location, 1, &value[0]);
}
void GLShader::setUniform(GLint location, const mat4& value){
	glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}
void GLShader::setUniform(GLint location, const GLint value){
	glUniform1i(location, value);
}
void GLShader::setUniform(GLint location, const GLuint value){
	glUniform1i(location, value);
}
void GLShader::setUniform(GLint location, const GLfloat value){
	glUniform1f(location, value);
}
void GLShader::setUniform(GLint location, const float* value){
	glUniformMatrix4fv(location, 1, GL_FALSE, &value[0]);
}