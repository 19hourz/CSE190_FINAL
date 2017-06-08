#ifndef _CUBE_H_
#define _CUBE_H_

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
// Use of degrees is deprecated. Use radians instead.
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Cube
{
public:
	Cube();
	Cube(int);
	~Cube();

	glm::mat4 toWorld;
	bool isCube = false;
	void draw(GLuint, glm::mat4, glm::mat4);
	void update();
	void scale(float);
	void movex(float);
	void movey(float);
	void movez(float);
	void reset();
	// These variables are needed for the shader program
	GLuint VBO, VAO, EBO, cubeVAO, skyboxVAO, skyboxTexture;
	GLuint uProjection, uModelview;
	GLuint skyboxProgram;
};

#endif