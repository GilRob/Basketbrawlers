#define _CRT_SECURE_NO_WARNINGS
#include "Mesh.h"
#include <fstream>
#include <iostream>
#include <MiniMath\Core.h>


#define CHAR_BUFFER_SIZE 128
#define BUFFER_OFFSET(i) ((char *)0 + (i))

struct MeshFace
{
	MeshFace()
	{
		vertices[0] = 0;
		vertices[1] = 0;
		vertices[2] = 0;

		texturesUVs[0] = 0;
		texturesUVs[1] = 0;
		texturesUVs[2] = 0;

		normals[0] = 0;
		normals[1] = 0;
		normals[2] = 0;
	}

	MeshFace(unsigned v1, unsigned v2, unsigned v3,
		unsigned t1, unsigned t2, unsigned t3,
		unsigned n1, unsigned n2, unsigned n3)
	{
		vertices[0] = v1;
		vertices[1] = v2;
		vertices[2] = v3;

		texturesUVs[0] = t1;
		texturesUVs[1] = t2;
		texturesUVs[2] = t3;

		normals[0] = n1;
		normals[1] = n2;
		normals[2] = n3;
	}

	unsigned vertices[3];
	unsigned texturesUVs[3];
	unsigned normals[3];
};


Mesh::Mesh()
{
	
}
Mesh::~Mesh()
{

}

//- Load a mesh, and send it to OpenGL
bool Mesh::LoadFromFile(const std::vector<std::string> &files)
{

	_NumFrames = files.size();

	VBO_Vertices = new GLuint[_NumFrames];
	VBO_UVs = new GLuint[_NumFrames];
	VBO_Normals = new GLuint[_NumFrames];
	glGenVertexArrays(1, &VAO);

	for (unsigned int c = 0; c < _NumFrames;++c)
	{

		std::ifstream input;
		input.open(files[c]);

		if (!input)
		{
			std::cout << "Could not open the file." << std::endl;
			return false;
		}

		char inputString[CHAR_BUFFER_SIZE];

		//Unique data
		std::vector<vec3> vertexData;
		std::vector<vec2> textureData;
		std::vector<vec3> normalData;
		//index/face data
		std::vector<MeshFace> faceData;
		//OpenGL ready data
		std::vector<float> unPackedVertexData;
		std::vector<float> unPackedTextureData;
		std::vector<float> unPackedNormalData;

		while (!input.eof())
		{
			input.getline(inputString, CHAR_BUFFER_SIZE);

			//strstr checks if one string is part of another
			//Returns a pointer to the place where "#" occurs in inputString
			//if it does not appear at all it is going to be nullptr
			//"#" is a comment
			if (std::strstr(inputString, "#") != nullptr)
			{
				//This line is a comment
				continue;
				//Continues to the top of the while loop
			}
			else if (std::strstr(inputString, "vn") != nullptr)
			{
				//This line as vertex data
				vec3 temp;
				//Checks for the letter vn and then three floats
				std::sscanf(inputString, "vn %f %f %f", &temp.x, &temp.y, &temp.z);
				normalData.push_back(temp);
			}
			else if (std::strstr(inputString, "vt") != nullptr)
			{
				//This line as vertex data
				vec2 temp;
				//Checks for the letter vt tand then two floats
				std::sscanf(inputString, "vt %f %f", &temp.x, &temp.y);
				textureData.push_back(temp);
			}
			else if (inputString[0] == 'v')
			{
				//This line as vertex data
				vec3 temp;
				//Checks for the letter v and then three floats
				std::sscanf(inputString, "v %f %f %f", &temp.x, &temp.y, &temp.z);
				vertexData.push_back(temp);
			}
			else if (inputString[0] == 'f')
			{
				//This line contains face data
				MeshFace temp;

				std::sscanf(inputString, "f %u/%u/%u %u/%u/%u %u/%u/%u",
					&temp.vertices[0], &temp.texturesUVs[0], &temp.normals[0],
					&temp.vertices[1], &temp.texturesUVs[1], &temp.normals[1],
					&temp.vertices[2], &temp.texturesUVs[2], &temp.normals[2]);

				faceData.push_back(temp);
			}
		}

		input.close();

		//unpack the data
		for (unsigned i = 0; i < faceData.size(); i++)
		{
			for (unsigned j = 0; j < 3; j++)
			{
				unPackedVertexData.push_back(vertexData[faceData[i].vertices[j] - 1].x);
				unPackedVertexData.push_back(vertexData[faceData[i].vertices[j] - 1].y);
				unPackedVertexData.push_back(vertexData[faceData[i].vertices[j] - 1].z);

				unPackedTextureData.push_back(textureData[faceData[i].texturesUVs[j] - 1].x);
				unPackedTextureData.push_back(textureData[faceData[i].texturesUVs[j] - 1].y);

				unPackedNormalData.push_back(normalData[faceData[i].normals[j] - 1].x);
				unPackedNormalData.push_back(normalData[faceData[i].normals[j] - 1].y);
				unPackedNormalData.push_back(normalData[faceData[i].normals[j] - 1].z);
			}
		}

		_NumFaces = faceData.size();
		_NumVertices = _NumFaces * 3;

		//Send data to OpenGl
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO_Vertices[c]);
		glGenBuffers(1, &VBO_UVs[c]);
		glGenBuffers(1, &VBO_Normals[c]);

		/*VAO keeps track of what you specify for all your VBOs
		and how they interact with shaders while you are uploading them to OpenGL.
		Instead of having to repeat the process of telling OpenGL what is inside every buffer
		and what it connects to and looks like. VAO will remember all of that
		so when you want to render you just bind the VAO and calling glDraw*/
		glBindVertexArray(VAO);


		glEnableVertexAttribArray(0);	//Vertex
		glEnableVertexAttribArray(1);	//UVs
		glEnableVertexAttribArray(2);	//Normals

		glBindBuffer(GL_ARRAY_BUFFER, VBO_Vertices[c]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * unPackedVertexData.size(), &unPackedVertexData[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));

		glBindBuffer(GL_ARRAY_BUFFER, VBO_UVs[c]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * unPackedTextureData.size(), &unPackedTextureData[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, BUFFER_OFFSET(0));

		glBindBuffer(GL_ARRAY_BUFFER, VBO_Normals[c]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * unPackedNormalData.size(), &unPackedNormalData[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));

		vertexData.clear();
		textureData.clear();
		faceData.clear();
		unPackedVertexData.clear();
		unPackedTextureData.clear();
		unPackedNormalData.clear();
	}

	//Cleanup
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

//- Release data from OpenGL (VRAM)
void Mesh::Unload()
{
	for (int c = 0; c < _NumFrames; ++c)
	{
		glDeleteBuffers(1, &VBO_Normals[c]);
		glDeleteBuffers(1, &VBO_UVs[c]);
		glDeleteBuffers(1, &VBO_Vertices[c]);
	}
	glDeleteVertexArrays(1, &VAO);

	VBO_Normals = 0;
	VBO_UVs = 0;
	VBO_Vertices = 0;
	VAO = 0;

	_NumFaces = 0;
	_NumVertices = 0;
}

unsigned int Mesh::GetNumFaces() const
{
	return _NumFaces;
}
unsigned int Mesh::GetNumVertices() const
{
	return _NumVertices;
}