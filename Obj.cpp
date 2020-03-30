#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <regex>

#include "Obj.h"

using namespace std;

void trimLine(string& str)
{
	// remove leading whitespaces from string
	str = regex_replace(str, regex("^\\s*"), "");
}

void decrementIndices(unsigned int* pIndices)
{
	for_each(
		pIndices,
		pIndices + 3,
		[](unsigned int& x) { x -= 1; }
	);
}

bool readMaterials(const string& filename, vector<Material*>& materials)
{
	ifstream mtlFile(filename);
	if (!mtlFile)
	{
		cerr << "Failed to load Mtl file" << endl;
		return false;
	}

	string mtlText((istreambuf_iterator<char>(mtlFile)),
		(istreambuf_iterator<char>()));
	mtlFile.close();

	stringstream buffer(mtlText);

	string line;
	string prefix;
	Material* pMat;
	while (!buffer.eof())
	{
		getline(buffer, line);
		trimLine(line);
		stringstream lineStream(line);
		lineStream >> prefix;
		// skip empty line
		if (prefix.length() == 0)
		{
			continue;
		}
		// skip comments
		if (prefix.find("#") != string::npos)
		{
			continue;
		}
		if (prefix == "newmtl")
		{
			pMat = new Material();
			materials.push_back(pMat);
			lineStream >> pMat->name;
		}
		if (prefix == "Ns")
		{
			lineStream >> pMat->Ns;
		}
		if (prefix == "Ni")
		{
			lineStream >> pMat->Ni;
		}
		if (prefix == "d")
		{
			lineStream >> pMat->d;
		}
		if (prefix == "Tr")
		{
			lineStream >> pMat->Tr;
		}
		if (prefix == "Tf")
		{
			lineStream >> pMat->Tf[0] >> pMat->Tf[1] >> pMat->Tf[2];
		}
		if (prefix == "illum")
		{
			lineStream >> pMat->illum;
		}
		if (prefix == "Ka")
		{
			lineStream >> pMat->Ka[0] >> pMat->Ka[1] >> pMat->Ka[2];
		}
		if (prefix == "Kd")
		{
			lineStream >> pMat->Kd[0] >> pMat->Kd[1] >> pMat->Kd[2];
		}
		if (prefix == "Ks")
		{
			lineStream >> pMat->Ks[0] >> pMat->Ks[1] >> pMat->Ks[2];
		}
		if (prefix == "Ke")
		{
			lineStream >> pMat->Ke[0] >> pMat->Ke[1] >> pMat->Ke[2];
		}
		if (prefix == "map_Ka")
		{
			lineStream >> pMat->map_Ka;
		}
		if (prefix == "map_Kd")
		{
			lineStream >> pMat->map_Kd;
		}
	}
	return true;
}

void ObjLoader::load(const string filename,
	vector<Material*>& materials,
	vector<Vertex>& vertices,
	vector<unsigned short>& indices
)
{
	vector<glm::vec3> positions;
	vector<glm::vec3> normals;
	vector<glm::vec2> uvs;
	Mesh* mesh = 0;
	vector<Face> faces;

	ifstream objFile(filename);
	string objText(
		(istreambuf_iterator<char>(objFile)),
		(istreambuf_iterator<char>())
	);
	objFile.close();
	stringstream buffer(objText);
	string line;
	string prefix;

	while (!buffer.eof())
	{
		getline(buffer, line);
		trimLine(line);
		istringstream lineStream(line);
		lineStream >> prefix;

		if (prefix == "#" || line.length() == 0)
		{
			continue;
		}

		if (prefix == "v")
		{
			glm::vec3 pos;
			lineStream >> pos.x >> pos.y >> pos.z;
			positions.push_back(pos); 
		}

		if (prefix == "vn")
		{
			glm::vec3 normal;
			lineStream >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		}

		if (prefix == "vt")
		{
			glm::vec3 uv;
			lineStream >> uv.x >> uv.y;
			uvs.push_back(uv);
		}

		if (prefix == "g")
		{
			string meshName;
			lineStream >> meshName;
			mesh = new Mesh(meshName);
		}

		if (prefix == "usemtl")
		{
			string mtlName;
			lineStream >> mtlName;
			vector<Material*>::const_iterator it = materials.begin();
			for (; it != materials.end(); it++)
			{
				if ((*it)->name == mtlName)
				{
					mesh->materialIndex = it - materials.begin();
					break;
				}
			}
		}

		if (prefix == "mtllib")
		{
			string dirname = filename.substr(
					0, filename.find_last_of('/') + 1
			);
			string mtlFilename;
			lineStream >> mtlFilename;
			mtlFilename = dirname + mtlFilename;
			readMaterials(mtlFilename, materials);
		}

		if (prefix == "f")
		{
			char sep; // "/" separator
			// Read three vertices
			Face f;
			for (int i = 0; i < 3; i++)
			{
				
				lineStream 	>> f.posIndices[i] >> sep
							>> f.uvIndices[i] >> sep
							>> f.normalIndices[i];
				
			}

			// Decrement indices by as as obj indices start from 1
			decrementIndices(f.posIndices);
			decrementIndices(f.normalIndices);
			decrementIndices(f.uvIndices);

			faces.push_back(f);
			materials[mesh->materialIndex]->faceIndices.push_back(
				faces.size() - 1);

			// if 4th vertex
			// create a new face using 0th and 2nd vertices
			// from previous face
			// lineStream >> sep;
			if (lineStream.str().length() > 0)
			{
				Face secondFace(f);
				secondFace.posIndices[1] = f.posIndices[2];
				secondFace.normalIndices[1] = f.normalIndices[2];
				secondFace.uvIndices[1] = f.uvIndices[2];
				lineStream 	>> secondFace.posIndices[2] >> sep
							>> secondFace.uvIndices[2] >> sep
							>> secondFace.normalIndices[2];

				secondFace.posIndices[2]--;
				secondFace.normalIndices[2]--;
				secondFace.uvIndices[2]--;

				faces.push_back(secondFace);
				materials[mesh->materialIndex]->faceIndices.push_back(
					faces.size() - 1);
			}
		}
	}

	// Get expanded list of vertices and indices
	// sorted by materials
	vector<Material*>::const_iterator it = materials.begin();
	int offset = 0;
	int count = 0;

	for (; it != materials.end(); it++)
	{
		Material* pMat = *it;
		vector<unsigned int>& faceIndices = pMat->faceIndices;
		for (int idx = 0; idx < faceIndices.size(); idx++)
		{
			Face& face = faces[faceIndices[idx]];
			for (int j = 0; j < 3; j++)
			{
				Vertex vert;

				vert.position = positions[face.posIndices[j]];
				vert.normal = normals[face.normalIndices[j]];
				vert.uv = uvs[face.uvIndices[j]];
				vertices.push_back(vert);
				indices.push_back(vertices.size() - 1);
				count++;
			}
		}

		pMat->offset = offset;
		pMat->count = count - offset;
		offset = count;
	}
}
