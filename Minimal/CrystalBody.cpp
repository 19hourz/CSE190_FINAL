//
//  16Terrain.cpp
//  West World
//
//  Created by Jiasheng Zhu on 3/19/17.
//  Copyright © 2017 Jiasheng Zhu. All rights reserved.
//

#include "CrystalBody.h"

CrystalBody::CrystalBody()
{
    toWorld = glm::mat4(1.0f);
    GLfloat blockSize = 1.5f;
    GLfloat variation = 0.0f;
    glm::vec3 normal;
    GLfloat ini_height = -3.0f;
    
    
    GLfloat vertices[6][3] = {
        {0.0f, -1.0f , 0.0f},
        {-0.25f, -0.5f, -0.25f},
        {0.25f, -0.5f, -0.25f},
        {0.25f, -0.50f, 0.25f},
        {-0.25f, -0.50f, 0.25f},
        {0.0f, 0.0f, 0.0f},
    };
    
    GLuint indices[8][3] = {
        {0, 1, 2},
        {0, 2, 3},
        {0, 3, 4},
        {0, 1, 4},
        {1, 2, 5},
        {2, 3, 5},
        {4, 3, 5},
        {1, 4, 5},
    };
    
    // Create array object and buffers. Remember to delete your buffers when the object is destroyed!
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &NAO);

    // Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
    // Consider the VAO as a container for all your buffers.
    glBindVertexArray(VAO);
    
    // Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
    // you want to draw, such as vertices, normals, colors, etc.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
    // the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Enable the usage of layout location 0 (check the vertex shader to see what this is)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
                          3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
                          GL_FLOAT, // What type these components are
                          GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
                          3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
                          (GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
    

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

CrystalBody::~CrystalBody()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void CrystalBody::draw(GLuint shaderProgram, glm::mat4 V, glm::mat4 P)
{
	glDisable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glUseProgram(shaderProgram);
    glm::mat4 modelview = V * toWorld;
    uProjection = glGetUniformLocation(shaderProgram, "projection");
    uModelview = glGetUniformLocation(shaderProgram, "modelview");
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, &P[0][0]);
    glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, numberIndices*6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void CrystalBody::update()
{
}

