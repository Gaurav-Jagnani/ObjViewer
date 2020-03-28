#ifndef SHADER_H
#define SHADER_H

#include <map>
#include <GL/glew.h>

using namespace std;


class Shader
{
	public:
		map<const char*, unsigned int> attributes;
		map<const char*, unsigned int> uniforms;

		void loadFromFile(GLenum shaderType, const char* filename);
		void loadFromString(GLenum shaderType, const char* source);

		void loadAttribLocation(const char* name);
		void loadUniformLocation(const char* name);
		void createAndLinkProgram();
		void use();
		void unUse();

	private:
		map<GLenum, unsigned int> m_shaders;
		unsigned int m_programId;
};

#endif