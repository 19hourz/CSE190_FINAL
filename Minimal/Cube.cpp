#include "Cube.h"
#include "Shader.h"
#include <iostream>
#include <vector>
GLuint texture_ID;
GLuint uv_ID;
GLuint cubeTexture;
#pragma warning (disable : 4996)
//! Load a ppm file from disk.
// @input filename The location of the PPM file.  If the file is not found, an error message
//		will be printed and this function will return 0
// @input width This will be modified to contain the width of the loaded image, or 0 if file not found
// @input height This will be modified to contain the height of the loaded image, or 0 if file not found
//
// @return Returns the RGB pixel data as interleaved unsigned chars (R0 G0 B0 R1 G1 B1 R2 G2 B2 .... etc) or 0 if an error ocured

unsigned char* loadPPM(const char* filename, int& width, int& height)
{
	const int BUFSIZE = 128;
	FILE* fp;
	unsigned int read;
	unsigned char* rawData;
	char buf[3][BUFSIZE];
	char* retval_fgets;
	size_t retval_sscanf;

	if ((fp = fopen(filename, "rb")) == NULL)
	{
		std::cerr << "error reading ppm file, could not locate " << filename << std::endl;
		width = 0;
		height = 0;
		return 0;
	}

	// Read magic number:
	retval_fgets = fgets(buf[0], BUFSIZE, fp);

	// Read width and height:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');
	retval_sscanf = sscanf(buf[0], "%s %s", buf[1], buf[2]);
	width = atoi(buf[1]);
	height = atoi(buf[2]);

	// Read maxval:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');

	// Read image data:
	rawData = new unsigned char[width * height * 3];
	read = fread(rawData, width * height * 3, 1, fp);
	fclose(fp);
	if (read != 1)
	{
		std::cerr << "error parsing ppm file, incomplete data" << std::endl;
		delete[] rawData;
		width = 0;
		height = 0;

		return 0;
	}

	return rawData;
}

// Loads a cubemap texture from 6 individual texture faces
// Order should be:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)? (CHECK THIS)
// -Z (back)?
GLuint loadCubemap(std::vector<const GLchar*> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (GLuint i = 0; i < faces.size(); i++)
	{
		image = loadPPM(faces[i], width, height);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
			GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
		);
	}
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}

// This function loads a texture from file. Note: texture loading functions like these are usually
// managed by a 'Resource Manager' that manages all resources (like textures, models, audio).
// For learning purposes we'll just define it as a utility function.
GLuint loadTexture(GLchar* path)
{
	//Generate texture ID and load texture data
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = loadPPM(path, width, height);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureID;
}

Cube::Cube()
{
	isCube = true;
	toWorld = glm::mat4(1.0f);

	cubeTexture = loadTexture("..\\res\\vr_test_pattern.ppm");
	
	GLfloat vertices[] = {
		// Positions           // Texture Coords
		//Back
		-0.5f, -0.5f,   -0.5f,   1.0f, 1.0f,
		0.5f,  -0.5f,   -0.5f,   0.0f, 1.0f,
		0.5f,  0.5f,    -0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,    -0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,   -0.5f,  1.0f, 0.0f,
		-0.5f,  -0.5f,  -0.5f,  1.0f, 1.0f,

		//Front
		-0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
		0.5f,  -0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,   0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,   0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  -0.5f,  0.5f,  0.0f, 1.0f,

		//Left
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  -0.5f,  0.0f, 0.0f,
		-0.5f,  -0.5f,  -0.5f,  0.0f, 1.0f,
		-0.5f,  -0.5f,  -0.5f,  0.0f, 1.0f,
		-0.5f,  -0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		//Right
		0.5f,  0.5f,  0.5f,    0.0f, 0.0f,
		0.5f,  0.5f,  -0.5f,   1.0f, 0.0f,
		0.5f,  -0.5f,  -0.5f,  1.0f, 1.0f,
		0.5f,  -0.5f,  -0.5f,  1.0f, 1.0f,
		0.5f,  -0.5f,  0.5f,   0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,    0.0f, 0.0f,



		//Bot
		-0.5f, -0.5f,  -0.5f,   1.0f, 0.0f,
		0.5f, -0.5f,  -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,   1.0f, 0.0f,

		//Top
		-0.5f,  0.5f,  -0.5f,  0.0f, 0.0f,
		0.5f,  0.5f, -0.5f,   1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f,  0.5f,  -0.5f,  0.0f, 0.0f
	};


	

	// Setup cube VAO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);
	

}

Cube::Cube(int eye) {
	skyboxProgram = LoadShaders(".\\skybox.vert", ".\\skybox.frag");
	// Set the object data (buffers, vertex attributes)
	GLfloat cubeVertices[] = {
		// Positions          // Normals
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};
	for (int i = 0; i < 216; ++i) {
	if(cubeVertices[i] == 0.5f || cubeVertices[i] == -0.5f)
	cubeVertices[i] = cubeVertices[i] * 100;
	}
	GLfloat skyboxVertices[] = {
		// Positions
		-10.0f,  10.0f, -10.0f,
		-10.0f, -10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,
		10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,

		-10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,

		10.0f, -10.0f, -10.0f,
		10.0f, -10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f,  10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,

		-10.0f, -10.0f,  10.0f,
		-10.0f,  10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,

		-10.0f,  10.0f, -10.0f,
		10.0f,  10.0f, -10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		-10.0f,  10.0f,  10.0f,
		-10.0f,  10.0f, -10.0f,

		-10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f,  10.0f,
		10.0f, -10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f,  10.0f,
		10.0f, -10.0f,  10.0f
	};

	// Setup cube VAO
	GLuint cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);
	// Setup skybox VAO
	GLuint skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

#pragma endregion

	// Cubemap (Skybox)
	std::vector<const GLchar*> faces;
	if (eye == 0) {
		faces.push_back("..\\res\\left-ppm\\nx.ppm");
		faces.push_back("..\\res\\left-ppm\\px.ppm");
		faces.push_back("..\\res\\left-ppm\\py.ppm");
		faces.push_back("..\\res\\left-ppm\\ny.ppm");
		faces.push_back("..\\res\\left-ppm\\nz.ppm");
		faces.push_back("..\\res\\left-ppm\\pz.ppm");
	}
	else if (eye == 1){
		faces.push_back("..\\res\\right-ppm\\nx.ppm");
		faces.push_back("..\\res\\right-ppm\\px.ppm");
		faces.push_back("..\\res\\right-ppm\\py.ppm");
		faces.push_back("..\\res\\right-ppm\\ny.ppm");
		faces.push_back("..\\res\\right-ppm\\nz.ppm");
		faces.push_back("..\\res\\right-ppm\\pz.ppm");
	}
	else if (eye == 2) {
		faces.push_back("..\\res\\bedroom\\nx.ppm");
		faces.push_back("..\\res\\bedroom\\px.ppm");
		faces.push_back("..\\res\\bedroom\\py.ppm");
		faces.push_back("..\\res\\bedroom\\ny.ppm");
		faces.push_back("..\\res\\bedroom\\nz.ppm");
		faces.push_back("..\\res\\bedroom\\pz.ppm");
	}

	skyboxTexture = loadCubemap(faces);
}

Cube::~Cube()
{
	// Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a 
	// large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Cube::draw(GLuint shaderProgram, glm::mat4 V, glm::mat4 P)
{

	if (isCube) {
		glDisable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		//glDepthFunc(GL_GREATER);
		glUseProgram(shaderProgram);
		// Calculate the combination of the model and view (camera inverse) matrices
		glm::mat4 modelview = V  * toWorld;
		uProjection = glGetUniformLocation(shaderProgram, "projection");
		uModelview = glGetUniformLocation(shaderProgram, "modelview");

		// Now send these values to the shader program
		glUniformMatrix4fv(uProjection, 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);


		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

	else {
		glDisable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		glUseProgram(skyboxProgram);
		// Cubes
		glBindVertexArray(cubeVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		// Draw skybox as last  // Change depth function so depth test passes when values are equal to depth buffer's content

		glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "view"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "projection"), 1, GL_FALSE, &P[0][0]);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		//    glActiveTexture(GL_TEXTURE0);
		//    glUniform1i(glGetUniformLocation(shaderProgram, "skybox"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
}

void Cube::update()
{
	//spin(1.0f);
}

GLfloat size = 1.0f;

void Cube::scale(float deg)
{
	if (size > 0.03 && size < 3.33) {
		toWorld = toWorld * glm::scale(glm::mat4(1.0f), glm::vec3(deg, deg, deg));
		size *= deg;
	}
}

void Cube::movex(float deg)
{
	toWorld = toWorld * glm::translate(glm::mat4(1.0f), glm::vec3(deg, 0.0f, 0.0f));
}

void Cube::movey(float deg)
{
	toWorld = toWorld * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, deg, 0.0f));
}

void Cube::movez(float deg)
{
	toWorld = toWorld * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, deg));
}

void Cube::reset() {
	size = 1.0f;
	toWorld = glm::mat4(1.0f);
}