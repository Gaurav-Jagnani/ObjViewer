#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <SOIL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Obj.h"

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "SOIL.lib")

#define GL_CHECK_ERRORS assert(glGetError() == GL_FALSE);

using namespace std;

//screen size
const int WIDTH  = 1280;
const int HEIGHT = 960;

std::vector<Material*> materials;
std::vector<Vertex> vertices;
std::vector<unsigned int> indices;

glm::mat4 MV(1);
glm::mat4 P(1);

const string objFile("media/blocks.obj");


Shader shader;


void onInit()
{
	shader.loadFromFile(GL_VERTEX_SHADER, "shaders/flatshader.vert");
	shader.loadFromFile(GL_FRAGMENT_SHADER, "shaders/flatshader.frag");
	shader.createAndLinkProgram();
	shader.use();
	shader.loadAttribLocation("vPos");
	// shader.loadAttribLocation("vUV");
	shader.loadUniformLocation("MV");
	shader.loadUniformLocation("P");
	// shader.loadUniformLocation("textureMap1");

	shader.unUse();

	// vao
	GLuint vaoId;
	GLuint vboId;
	GLuint eboId;

	ObjLoader::load(objFile,
					materials,
					vertices,
					indices
	);

	glGenVertexArrays(1, &vaoId);
	glBindVertexArray(vaoId);

	glGenBuffers(1, &vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData (GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(),
		&(vertices[0].position.x), GL_STATIC_DRAW);
	glEnableVertexAttribArray(shader.attributes["vPos"]);
	glVertexAttribPointer(shader.attributes["vPos"], 3,
		GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	// glEnableVertexAttribArray(shader.attributes["vUV"]);
	// glVertexAttribPointer(shader.attributes["vUV"],
	// 	3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, uv));

	glGenBuffers(1, &eboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),
		&indices[0], GL_STATIC_DRAW);

	GLuint texId[2];
	string filenames[] = { "download.jpg", "Img.png" };
	int texture_width;
	int texture_height;
	int channels;
	for (int i = 0; i < 2; i++)
	{
		GLubyte* pData = SOIL_load_image(filenames[i].c_str(), &texture_width, &texture_height, &channels, SOIL_LOAD_AUTO);
		glActiveTexture(GL_TEXTURE0 + i);
		glGenTextures(2, texId);
		glBindTexture(GL_TEXTURE_2D, texId[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		//allocate texture 
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, pData);
		SOIL_free_image_data(pData);
	}
}

void OnResize(int w, int h) {
	//set the viewport
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	//setup the projection matrix
	P = glm::perspective(glm::radians(45.0f),(float)w/h, 0.1f,1000.0f);
}


void onRender()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.use();
	
	glUniformMatrix4fv(shader.uniforms["MV"], 1, GL_FALSE, glm::value_ptr(MV));
	glUniformMatrix4fv(shader.uniforms["P"], 1, GL_FALSE, glm::value_ptr(P));
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);

	shader.unUse();

	glutSwapBuffers();
}


int main(int argc, char** argv) {
	//freeglut initialization calls
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitContextVersion (3, 3);
	glutInitContextFlags (GLUT_CORE_PROFILE | GLUT_DEBUG);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Simple triangle - OpenGL 3.3");

	//glew initialization
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cerr<<"Error: "<<glewGetErrorString(err)<<endl;
	}
	else
	{
		if (GLEW_VERSION_3_3)
		{
			cout<<"Driver supports OpenGL 3.3\nDetails:"<<endl;
		}
	}

	//opengl initialization
	onInit();

	//callback hooks
	glutReshapeFunc(OnResize); 
	glutDisplayFunc(onRender);

	//main loop call
	glutMainLoop();

	return 0;
}

