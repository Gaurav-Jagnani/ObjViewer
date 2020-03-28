#ifndef OBJ_H
#define OBJ_H

#include <string>
#include <vector>

#include <glm/glm.hpp>


struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 uv;
};

struct Face
{
	unsigned int posIndices[3],
					normalIndices[3],
					uvIndices[3],
					materialIndex;
};

struct Mesh
{
	Mesh(const std::string meshName): name(meshName) {}
		std::string name;

	unsigned int materialIndex;
};

struct Material
{
	std::string name;

	float Ns;
	float Ni;
	float d;
	float Tr;
	float Tf[3];
	int illum;
	float Ka[3];
	float Kd[3];
	float Ks[3];
	float Ke[3];
	std::string map_Ka;
	std::string map_Kd;

	std::vector<unsigned int> faceIndices;
	unsigned int offset;
	unsigned int count;
};


class ObjLoader
{
	public:
		static void load(const std::string filename,
						std::vector<Material*>& materials,
						std::vector<Vertex>& vertices,
						std::vector<unsigned int>& indices
		);
};

#endif