#define _CRT_SECURE_NO_WARNINGS
#include "Mesh.h"
#include <fstream>
#include <iostream>
//#include <MiniMath\Core.h>


#define CHAR_BUFFER_SIZE 128
#define BUFFER_OFFSET(i) ((char *)0 + (i))
#define METADATASIZE sizeof(int) * 2

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

struct MeshFaceFloat
{
	MeshFaceFloat()
	{
		vertices[0] = 0;
		vertices[1] = 0;
		vertices[2] = 0;

		textureUVs[0] = 0;
		textureUVs[1] = 0;
		textureUVs[2] = 0;

		normals[0] = 0;
		normals[1] = 0;
		normals[2] = 0;

	}
	MeshFaceFloat(float v1, float v2, float v3, float v4,
		float t1, float t2, float t3, float t4,
		float n1, float n2, float n3, float n4)
	{
		vertices[0] = v1;
		vertices[1] = v2;
		vertices[2] = v3;
		vertices[3] = v4;

		textureUVs[0] = t1;
		textureUVs[1] = t2;
		textureUVs[2] = t3;
		textureUVs[3] = t4;

		normals[0] = n1;
		normals[1] = n2;
		normals[2] = n3;
		normals[3] = n4;
	}
	union
	{
		struct
		{
			float vertices[4];
			float textureUVs[4];
			float normals[4];
		};

		float face[16];
	};

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

	glBindVertexArray(VAO);

	for (unsigned int c = 0; c < _NumFrames; ++c)
	{
		glGenBuffers(1, &VBO_Vertices[c]);
		glGenBuffers(1, &VBO_UVs[c]);
		glGenBuffers(1, &VBO_Normals[c]);

		std::ifstream input;
		input.open(files[c]);

		if (!input)
		{
			std::cout << "Could not open the file." << std::endl;
			return false;
		}

		char inputString[CHAR_BUFFER_SIZE];

		//Unique data
		std::vector<glm::vec3> vertexData;
		std::vector<glm::vec2> textureData;
		std::vector<glm::vec3> normalData;
		//index/face data
		std::vector<MeshFace> faceData;
		//OpenGL ready data
		std::vector<float> unPackedVertexData;
		std::vector<float> unPackedTextureData;
		std::vector<float> unPackedNormalData;

		std::vector<MeshFaceFloat> unPackedFaceData;

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
				glm::vec3 temp;
				//Checks for the letter vn and then three floats
				std::sscanf(inputString, "vn %f %f %f", &temp.x, &temp.y, &temp.z);
				normalData.push_back(temp);
			}
			else if (std::strstr(inputString, "vt") != nullptr)
			{
				//This line as vertex data
				glm::vec2 temp;
				//Checks for the letter vt tand then two floats
				std::sscanf(inputString, "vt %f %f", &temp.x, &temp.y);
				textureData.push_back(temp);
			}
			else if (inputString[0] == 'v')
			{
				//This line as vertex data
				glm::vec3 temp;
				//Checks for the letter v and then three floats
				std::sscanf(inputString, "v %f %f %f", &temp.x, &temp.y, &temp.z);
				vertexData.push_back(temp);
			}
			else if (inputString[0] == 'f')
			{
				//This line contains face data
				MeshFace temp;

				int numSuccess = std::sscanf(inputString, "f %u/%u/%u %u/%u/%u %u/%u/%u",
					&temp.vertices[0], &temp.texturesUVs[0], &temp.normals[0],
					&temp.vertices[1], &temp.texturesUVs[1], &temp.normals[1],
					&temp.vertices[2], &temp.texturesUVs[2], &temp.normals[2]);

				if (numSuccess < 9)
				{
					numSuccess = std::sscanf(inputString, "f %u//%u %u//%u %u//%u",
						&temp.vertices[0], &temp.normals[0],
						&temp.vertices[1], &temp.normals[1],
						&temp.vertices[2], &temp.normals[2]);
					temp.texturesUVs[0] = 1;
					temp.texturesUVs[1] = 1;
					temp.texturesUVs[2] = 1;

					faceData.push_back(temp);
					if (numSuccess < 6)
					{
						std::cout << "WHOOPS";
					}
				}
				else
				{
					faceData.push_back(temp);
				}
			}
		}

		input.close();

		if (textureData.size() < 2)
		{
			textureData.push_back(glm::vec2());
		}

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

		unPackedFaceData.resize(faceData.size());

		_NumFaces = faceData.size();
		_NumVertices = _NumFaces * 3;

		//Send data to OpenGl
		/*glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO_Vertices[c]);
		glGenBuffers(1, &VBO_UVs[c]);
		glGenBuffers(1, &VBO_Normals[c]);*/

		/*VAO keeps track of what you specify for all your VBOs
		and how they interact with shaders while you are uploading them to OpenGL.
		Instead of having to repeat the process of telling OpenGL what is inside every buffer
		and what it connects to and looks like. VAO will remember all of that
		so when you want to render you just bind the VAO and calling glDraw*/
		//glBindVertexArray(VAO);

		unsigned int indexOffset = c * 3; // We have 3 VBOs

		glEnableVertexAttribArray(0 + indexOffset);
		glEnableVertexAttribArray(1 + indexOffset);
		glEnableVertexAttribArray(2 + indexOffset);

		/*glEnableVertexAttribArray(0);	//Vertex
		glEnableVertexAttribArray(1);	//UVs
		glEnableVertexAttribArray(2);	//Normals*/

		glBindBuffer(GL_ARRAY_BUFFER, VBO_Vertices[c]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * unPackedVertexData.size(), &unPackedVertexData[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0 + indexOffset, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));

		glBindBuffer(GL_ARRAY_BUFFER, VBO_UVs[c]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * unPackedTextureData.size(), &unPackedTextureData[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1 + indexOffset, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, BUFFER_OFFSET(0));

		glBindBuffer(GL_ARRAY_BUFFER, VBO_Normals[c]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * unPackedNormalData.size(), &unPackedNormalData[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)2 + indexOffset, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));

		vertexData.clear();
		textureData.clear();
		faceData.clear();
		unPackedVertexData.clear();
		unPackedTextureData.clear();
		unPackedNormalData.clear();
		unPackedFaceData.clear();
	}

	//Cleanup
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

//- Release data from OpenGL (VRAM)
void Mesh::Unload()
{
	for (unsigned int c = 0; c < _NumFrames; ++c)
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


//this union defines the metadata for a given binary file into a char array to be read by a binary file and vice versa @BINARY LOADER
union MetaUnion {
public:
	struct
	{
		int faceSize; //number of faces in the bin file
		int dataSize; //size of each face in bytes
	};

	char charArray[sizeof(int) * 2]; //char redefinition variable
};

//this union defines the data for a single face into a char array to be read by the binary file, and vice versa @BINARY LOADER
union FaceUnion
{
public:
	struct //all data in a face
	{
		//vert1
		float vx1;
		float vy1;
		float vz1;
		float tx1;
		float ty1;
		float nx1;
		float ny1;
		float nz1;
		//vert2
		float vx2;
		float vy2;
		float vz2;
		float tx2;
		float ty2;
		float nx2;
		float ny2;
		float nz2;
		//vert3
		float vx3;
		float vy3;
		float vz3;
		float tx3;
		float ty3;
		float nx3;
		float ny3;
		float nz3;
	};

	char charArray[sizeof(float) * 24]; //char redefinition variable
};

//creates a .bin of a .obj file @BINARY LOADER
bool Mesh::CreateBinary(const std::string &file)
{
	std::ifstream input;
	input.open(file + ".obj");

	if (!input)
	{
		std::cout << "Could not open the file." << std::endl;
		return false;
	}

	//parsing starts here @YOUTUBE
#pragma region Parse
	char inputString[CHAR_BUFFER_SIZE];

	//Unique data
	std::vector<glm::vec3> vertexData;
	std::vector<glm::vec2> textureData;
	std::vector<glm::vec3> normalData;
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
			glm::vec3 temp;
			//Checks for the letter vn and then three floats
			std::sscanf(inputString, "vn %f %f %f", &temp.x, &temp.y, &temp.z);
			normalData.push_back(temp);
		}
		else if (std::strstr(inputString, "vt") != nullptr)
		{
			//This line as vertex data
			glm::vec2 temp;
			//Checks for the letter vt tand then two floats
			std::sscanf(inputString, "vt %f %f", &temp.x, &temp.y);
			textureData.push_back(temp);
		}
		else if (std::strstr(inputString, "v") != nullptr)
		{
			//This line as vertex data
			glm::vec3 temp;
			//Checks for the letter v and then three floats
			std::sscanf(inputString, "v %f %f %f", &temp.x, &temp.y, &temp.z);
			vertexData.push_back(temp);
		}
		else if (std::strstr(inputString, "f") != nullptr)
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


#pragma endregion Parse



	//bin file creation starts here @BINARY LOADER

	//create a file for writing
	std::ofstream myFile(file + ".bin", std::ios::out | std::ios::binary);

	//specify metadata
	MetaUnion metadata;
	metadata.faceSize = faceData.size(); // number of faces in a given object
	metadata.dataSize = (sizeof(float) * 24); // bytes of data in each face

	//write the metadata in first
	myFile.write(metadata.charArray, METADATASIZE);

	//unpack the data into faceunions
	for (unsigned i = 1; i < faceData.size(); i++)
	{
		FaceUnion currFace;

		//all properties for vertice 1
		currFace.vx1 = vertexData[faceData[i].vertices[0] - 1].x;
		currFace.vy1 = vertexData[faceData[i].vertices[0] - 1].y;
		currFace.vz1 = vertexData[faceData[i].vertices[0] - 1].z;
		currFace.tx1 = textureData[faceData[i].texturesUVs[0] - 1].x;
		currFace.ty1 = textureData[faceData[i].texturesUVs[0] - 1].y;
		currFace.nx1 = normalData[faceData[i].normals[0] - 1].x;
		currFace.ny1 = normalData[faceData[i].normals[0] - 1].y;
		currFace.nz1 = normalData[faceData[i].normals[0] - 1].z;

		//all properties for vertice 2
		currFace.vx2 = vertexData[faceData[i].vertices[1] - 1].x;
		currFace.vy2 = vertexData[faceData[i].vertices[1] - 1].y;
		currFace.vz2 = vertexData[faceData[i].vertices[1] - 1].z;
		currFace.tx2 = textureData[faceData[i].texturesUVs[1] - 1].x;
		currFace.ty2 = textureData[faceData[i].texturesUVs[1] - 1].y;
		currFace.nx2 = normalData[faceData[i].normals[1] - 1].x;
		currFace.ny2 = normalData[faceData[i].normals[1] - 1].y;
		currFace.nz2 = normalData[faceData[i].normals[1] - 1].z;

		//all properties for vertice 3
		currFace.vx3 = vertexData[faceData[i].vertices[2] - 1].x;
		currFace.vy3 = vertexData[faceData[i].vertices[2] - 1].y;
		currFace.vz3 = vertexData[faceData[i].vertices[2] - 1].z;
		currFace.tx3 = textureData[faceData[i].texturesUVs[2] - 1].x;
		currFace.ty3 = textureData[faceData[i].texturesUVs[2] - 1].y;
		currFace.nx3 = normalData[faceData[i].normals[2] - 1].x;
		currFace.ny3 = normalData[faceData[i].normals[2] - 1].y;
		currFace.nz3 = normalData[faceData[i].normals[2] - 1].z;

		//write to binary file
		myFile.write(currFace.charArray, metadata.dataSize);
	}

	//cleanup
	vertexData.clear();
	textureData.clear();
	normalData.clear();
	faceData.clear();

	//close the file
	myFile.close();
}