#include "stdafx.h"
#include "Utility.h"
#include <fstream>
#include <string>
#include <vector>

bool Utility::loadShader(const std::string shaderName, GLuint &outID) {
	// Shader File Paths
	std::string vertexShaderPathString = "shaders/" + shaderName + ".vert";
	std::string fragmentShaderPathString = "shaders/" + shaderName + ".frag";

	std::cout << "... Loading shader \"" << shaderName << "\"" << std::endl;

	const char* vertexShaderPath = vertexShaderPathString.c_str();
	const char* fragmentShaderPath = fragmentShaderPathString.c_str();
	// Create the shaders
	const GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	const GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the vertex shader
	std::string vertexShaderCode;
	std::ifstream vertexShaderStream(vertexShaderPath, std::ios::in);
	if (vertexShaderStream.is_open()) {
		std::string line = "";
		while (std::getline(vertexShaderStream, line)) {
			vertexShaderCode += "\n" + line;
		}
		vertexShaderStream.close();
	}
	else {
		std::cerr << "Can not open " << vertexShaderPath << std::endl;
		return false;
	}

	// Read the fragment shader
	std::string fragmentShaderCode;
	std::ifstream fragmentShaderStream(fragmentShaderPath, std::ios::in);
	if (fragmentShaderStream.is_open()) {
		std::string line = "";
		while (std::getline(fragmentShaderStream, line)) {
			fragmentShaderCode += "\n" + line;
		}
		fragmentShaderStream.close();
	}
	else {
		std::cerr << "Can not open " << fragmentShaderPath << std::endl;
		return false;
	}

	GLint result = GL_FALSE;
	int infoLogLength;

	// Compile vertex shader
	std::cout << "* Compiling vertex shader: " << vertexShaderPath << std::endl;
	char const *vertexShaderPointer = vertexShaderCode.c_str();

	glShaderSource(vertexShaderID, 1, &vertexShaderPointer, nullptr);
	glCompileShader(vertexShaderID);

	// Check if everything went correct
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		std::vector<char> vertexShaderErrorMessage(infoLogLength + 1);
		glGetShaderInfoLog(vertexShaderID, infoLogLength, nullptr, &vertexShaderErrorMessage[0]);
		char* errorMsg = &vertexShaderErrorMessage[0];
		printf("%s\n", errorMsg);
		return false;
	}

	// Compile fragment shader
	std::cout << "* Compiling fragment shader: " << fragmentShaderPath << std::endl;
	char const *fragmentSourcePointer = fragmentShaderCode.c_str();
	glShaderSource(fragmentShaderID, 1, &fragmentSourcePointer, nullptr);
	glCompileShader(fragmentShaderID);

	// Check if everything went correct
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		std::vector<char> fragmentShaderErrorMessage(infoLogLength + 1);
		glGetShaderInfoLog(fragmentShaderID, infoLogLength, nullptr, &fragmentShaderErrorMessage[0]);
		char* errorMsg = &fragmentShaderErrorMessage[0];
		printf("Error: %s\n", errorMsg);
		return false;
	}

	// Link the shaders to the program
	printf("* Linking shader programm with vert & frag shader\n");
	const GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	// Check if everything went correct
	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		std::vector<char> programErrorMessage(infoLogLength + 1);
		glGetProgramInfoLog(programID, infoLogLength, nullptr, &programErrorMessage[0]);
		char* errorMsg = &programErrorMessage[0];
		printf("%s\n", errorMsg);
		return false;
	}

	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	outID = programID;
	return true;
}
bool Utility::getQuadPrimitiveData(std::vector<glm::vec3>& _vertexBufferData, std::vector<glm::vec2>& _uvBufferData, std::vector<glm::vec3>& _normalsBufferData, std::vector<unsigned short>& _indicesBufferData) {
	_vertexBufferData = std::vector<glm::vec3>();
	_vertexBufferData.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	_vertexBufferData.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	_vertexBufferData.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	_vertexBufferData.push_back(glm::vec3(1.0f, 1.0f, 0.0f));

	_uvBufferData = std::vector<glm::vec2>();
	_uvBufferData.push_back(glm::vec2(0.0f, 1.0f));
	_uvBufferData.push_back(glm::vec2(1.0f, 1.0f));
	_uvBufferData.push_back(glm::vec2(0.0f, 0.0f));
	_uvBufferData.push_back(glm::vec2(1.0f, 0.0f));
	
	_normalsBufferData = std::vector<glm::vec3>();
	_normalsBufferData.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	_normalsBufferData.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	_normalsBufferData.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	_normalsBufferData.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

	_indicesBufferData = std::vector<unsigned short>();
	_indicesBufferData.push_back(0);
	_indicesBufferData.push_back(1);
	_indicesBufferData.push_back(2);
	_indicesBufferData.push_back(1);
	_indicesBufferData.push_back(3);
	_indicesBufferData.push_back(2);

	return true;
}
bool Utility::loadObj(const char *path, std::vector<glm::vec3> &_outVertecies, std::vector<glm::vec2> &_outUVs, std::vector<glm::vec3> &_outNormals)
{
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> tempVertecies;
	std::vector<glm::vec2> tempUVs;
	std::vector<glm::vec3> tempNormals;

	FILE *file = fopen(path, "r");
	if (file == nullptr) {
		printf("Can not open file at %s", path);
		return false;
	}

	char lineHeader[128];
	int res;
	// while end of file is not reached, scan the file.
	// Store all scanned verts, uvs and normals temporaray
	// As OBJ compresses the file with indexing 
	//	(face is not made of verts directly but through vert indecies, allowing for multiuse)
	// I need to sort them afterwards
	while ((res = fscanf(file, "%s", lineHeader)) != EOF) {
		// parse what was scanned

		if (strcmp(lineHeader, "v") == 0) {
			// Case vertex
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			tempVertecies.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			// Case UV
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			tempUVs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			// Case Normal
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			tempNormals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
				&vertexIndex[0], &uvIndex[0], &normalIndex[0],
				&vertexIndex[1], &uvIndex[1], &normalIndex[1],
				&vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("OBJ file has to contain exactly only verts, uvs & vertex normals for this parser!");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}

	}
	// Indexing [Care: OBJ starts indexes at 1]
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {
		unsigned int vertindex = vertexIndices[i];
		glm::vec3 vert = tempVertecies[vertindex - 1];
		_outVertecies.push_back(vert);
	}
	for (unsigned int i = 0; i < uvIndices.size(); i++) {
		unsigned int uvIndex = uvIndices[i];
		glm::vec2 uv = tempUVs[uvIndex - 1];
		_outUVs.push_back(uv);
	}
	for (unsigned int i = 0; i < normalIndices.size(); i++) {
		unsigned int normalIndex = normalIndices[i];
		glm::vec3 normal = tempNormals[normalIndex - 1];
		_outNormals.push_back(normal);
	}

	return true;
}

/* Region: Indexing VBO  */
struct PackedVertex {
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;

	/* Compare of the memory of this packed vertex equals the memory of the other packet vertex */
	bool operator<(const PackedVertex that) const {
		return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
	};
};
bool getSimilarVertexIndex_fast(PackedVertex & packed, std::map<PackedVertex, unsigned short> & VertexToOutIndex, unsigned short & result)
{
	std::map<PackedVertex, unsigned short>::iterator it = VertexToOutIndex.find(packed);
	if (it == VertexToOutIndex.end()) {
		return false;
	}
	else {
		result = it->second;
		return true;
	}
}
bool Utility::indexVbo(std::vector<glm::vec3>& _inVertices, std::vector<glm::vec2>& _inUVs,
	std::vector<glm::vec3>& _inNormals, std::vector<unsigned short>& _outIndices, std::vector<glm::vec3>& _outVertices,
	std::vector<glm::vec2>& _outUvs, std::vector<glm::vec3>& _outNormals) {

	/* Store already found verts */
	std::map<PackedVertex, unsigned short> VertexToOutIndex;

	// For each input vertex
	for (unsigned int i = 0; i<_inVertices.size(); i++) {

		PackedVertex packed = { _inVertices[i], _inUVs[i], _inNormals[i] };

		// Try to find a similar vertex in out_XXXX
		unsigned short index;
		bool found = getSimilarVertexIndex_fast(packed, VertexToOutIndex, index);
		if (found) { // A similar vertex is already in the VBO, use it instead !
			_outIndices.push_back(index);
		}
		else { // If not, it needs to be added in the output data.
			_outVertices.push_back(_inVertices[i]);
			_outUvs.push_back(_inUVs[i]);
			_outNormals.push_back(_inNormals[i]);
			unsigned short newindex = (unsigned short)_outVertices.size() - 1;
			_outIndices.push_back(newindex);
			VertexToOutIndex[packed] = newindex;
		}
	}

	return true;
}


glm::mat4x4 Utility::translationMatrix(const glm::vec3 _position)
{
	glm::mat4x4 m = glm::mat4x4(1.0f);
	m[3][0] = _position.x;
	m[3][1] = _position.y;
	m[3][2] = _position.z;
	return m;
}

glm::mat4x4 Utility::scaleMatrix(const glm::vec3 _scale)
{
	glm::mat4x4 m = glm::mat4x4(1.0f);
	m[0][0] = _scale.x;
	m[1][1] = _scale.y;
	m[2][2] = _scale.z;
	return m;
}


void xxx() {

}