#include <iostream>
#include <sstream>
#include <fstream>

#include "Shader.h"
using namespace std;


void Shader::loadFromFile(GLenum shaderType, const char* filename)
{
	ifstream infile(filename);
	if (!infile)
	{
		cerr << "Failed to load shader from " << filename << endl;
		return;
	}

	stringstream ss;
	ss << infile.rdbuf();
	loadFromString(shaderType, ss.str().c_str());
}


void Shader::loadFromString(GLenum shaderType, const char* source)
{
	unsigned int shaderId =	glCreateShader(shaderType);
	glShaderSource(shaderId, 1, &source, NULL);
	glCompileShader(shaderId);

	// check for compile success
	GLint status;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetShaderInfoLog(
			shaderId,
			infoLogLength,
			NULL,
			infoLog
		);
		cerr << "Compile log: " << infoLog << endl;
		delete[] infoLog;
		return;
	}

	m_shaders[shaderType] = shaderId;
}


void Shader::loadAttribLocation(const char* name)
{
	attributes[name] = glGetAttribLocation(m_programId, name);
}


void Shader::loadUniformLocation(const char* name)
{
	uniforms[name] = glGetUniformLocation(m_programId, name);
}


void Shader::createAndLinkProgram()
{
	m_programId = glCreateProgram();
	map<GLenum, unsigned int>::const_iterator it = m_shaders.begin();
	if (m_shaders.find(GL_VERTEX_SHADER) != m_shaders.end())
	{
		glAttachShader(m_programId, m_shaders[GL_VERTEX_SHADER]);
	}
	if (m_shaders.find(GL_FRAGMENT_SHADER) != m_shaders.end())
	{
		glAttachShader(m_programId, m_shaders[GL_FRAGMENT_SHADER]);
	}
	// for (; it != m_shaders.end(); it++)
	// {
	// 	glAttachShader(m_programId, it->second);
	// 	cout << "fdffd " << it->second << endl;
	// }

	glLinkProgram(m_programId);
	// check for errors
	GLint status;
	glGetProgramiv(m_programId, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(m_programId, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(
			m_programId,
			infoLogLength,
			NULL,
			infoLog
		);

		cerr << "Shader Linking log: " << infoLog << endl;
		delete[] infoLog;
		return;
	}

	for (; it != m_shaders.end(); it++)
	{
		glDeleteShader(it->second);
	}
}


void Shader::use()
{
	glUseProgram(m_programId);
}


void Shader::unUse()
{
	glUseProgram(0);
}