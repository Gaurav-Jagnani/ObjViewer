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

vector<Material*> materials;
vector<Vertex> vertices;
vector<unsigned short> indices;
vector<Mesh*> meshes;
vector<int> textures;

glm::mat4 MV(1);
glm::mat4 P(1);

const string objFile("media/model.obj");

Shader shader;


void onInit()
{
	shader.loadFromFile(GL_VERTEX_SHADER, "shaders/shader.vert");
	shader.loadFromFile(GL_FRAGMENT_SHADER, "shaders/shader.frag");
	shader.createAndLinkProgram();
	shader.use();
	shader.loadAttribLocation("vPos");
	shader.loadAttribLocation("vUV");
	shader.loadUniformLocation("MV");
	shader.loadUniformLocation("P");
	shader.loadUniformLocation("textureMap");
	shader.loadUniformLocation("useDefaultColor");

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
	glBufferData (GL_ARRAY_BUFFER, sizeof(Vertex)*(vertices.size()),
		&(vertices[0].position.x), GL_STATIC_DRAW);
	glEnableVertexAttribArray(shader.attributes["vPos"]);
	glVertexAttribPointer(shader.attributes["vPos"], 3,
		GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	glEnableVertexAttribArray(shader.attributes["vUV"]);
	glVertexAttribPointer(shader.attributes["vUV"],
		2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, uv));

	// Load all textures
	for (Material* pMat : materials)
	{
		if (pMat->map_Kd == "")
		{
			cout << pMat->name;
			textures.push_back(0);
			return;
		}

		GLuint texId;
		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);
		int texture_width;
		int texture_height;
		int channels;

		string dirname = objFile.substr(
					0, objFile.find_last_of('/') + 1
		);
		string texFilename(pMat->map_Kd);
		texFilename = dirname + texFilename;
		GLubyte* pData = SOIL_load_image(texFilename.c_str(),
			&texture_width,
			&texture_height,
			&channels,
			SOIL_LOAD_AUTO
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		//allocate texture 
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, pData);
		SOIL_free_image_data(pData);

		textures.push_back(texId);
	}
}

void OnResize(int w, int h) {
	//set the viewport
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	//setup the projection matrix
	P = glm::perspective(glm::radians(90.0f),(float)w/h, 0.1f,1000.0f);
}


void onRender()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.use();

	//set the camera
	glm::mat4 T		= glm::translate(glm::mat4(1.0f),glm::vec3(0.f, 0.0f, -5.0f));
	glm::mat4 MV(T);
	glUniformMatrix4fv(shader.uniforms["MV"], 1, GL_FALSE, glm::value_ptr(MV));
	glUniformMatrix4fv(shader.uniforms["P"], 1, GL_FALSE, glm::value_ptr(P));
	glUniform1i(shader.uniforms["textureMap"], 0);

	for (int i = 0; i < materials.size(); i++)
	{
		Material* pMat = materials[i];
		if (pMat->map_Kd != "")
		{
			glUniform1f(shader.uniforms["useDefaultColor"], 0);
			glBindTexture(GL_TEXTURE_2D, textures[i]);
		}
		else
		{
			glUniform1f(shader.uniforms["useDefaultColor"], 1);
		}


		glDrawElements(GL_TRIANGLES, 
			pMat->count,
			GL_UNSIGNED_SHORT,
			(const GLvoid*)(&indices[pMat->offset])
		);

	}

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

	glutReshapeFunc(OnResize); 
	glutDisplayFunc(onRender);

	//main loop call
	glutMainLoop();

	return 0;
}

