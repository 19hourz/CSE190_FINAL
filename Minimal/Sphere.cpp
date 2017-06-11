#include "sphere.h"

#include <vector>
#include <iostream>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>



// This function initializes the properties of a sphere. Only sets up the buffer attributes,
// actual vertices and normals are assigned later.
void Sphere::initSphere() {
    //glGenVertexArrays(1, &VAOs);
    glGenVertexArrays(1, &VAOs);
    
    glBindVertexArray(VAOs);
    // Bind vertices to layout location 0
    glGenBuffers(1, &VBOs);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs);
    glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW); // Don't give it any data
    glEnableVertexAttribArray(0); // This allows usage of layout location 0 in the vertex shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    
    // Bind normals to layout location 1
    glGenBuffers(1, &NBOs);
    glBindBuffer(GL_ARRAY_BUFFER, NBOs);
    glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW); // Don't give it any data
    glEnableVertexAttribArray(1); // This allows usage of layout location 1 in the vertex shader
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


Sphere::Sphere(float radius, int stacks, int slices, vec3 position)
{
    toWorld = mat4(1.0f);
    toWorld = translate(mat4(1.0f), vec3(position.x,position.y,position.z)) * toWorld;
    this->position = position;
    initSphere();

    // No need to generate the sphere again if the previous rendering already
    // used the same number of stacks and slices
    sphereVertices.clear();
    sphereNormals.clear();
    float fstacks = (float) stacks;
    float fslices = (float) slices;
    float pi = glm::pi<float>();
    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < stacks; j++) {
            // Top left
            sphereVertices.push_back(vec3(
                                          radius * -cos(2.0f * pi * i / fstacks) * sin(pi * (j + 1.0f) / fslices),
                                          radius * -cos(pi * (j + 1.0f) / fslices),
                                          radius * sin(2.0f * pi * i / fstacks) * sin(pi * (j + 1.0f) / fslices)));
            sphereNormals.push_back(glm::normalize(vec3(
                                                        -cos(2.0f * pi * i / fstacks) * sin(pi * (j + 1.0f) / fslices),
                                                        -cos(pi * (j + 1.0f) / fslices),
                                                        sin(2.0f * pi * i / fstacks) * sin(pi * (j + 1.0f) / fslices))));
            // Top right
            sphereVertices.push_back(vec3(
                                          radius * -cos(2.0f * pi * (i + 1.0) / fstacks) * sin(pi * (j + 1.0) / fslices),
                                          radius * -cos(pi * (j + 1.0) / fslices),
                                          radius * sin(2.0f * pi * (i + 1.0) / fstacks) * sin(pi * (j + 1.0) / fslices)));
            sphereNormals.push_back(glm::normalize(vec3(
                                                        -cos(2.0f * pi * (i + 1.0) / fstacks) * sin(pi * (j + 1.0) / fslices),
                                                        -cos(pi * (j + 1.0) / fslices),
                                                        sin(2.0f * pi * (i + 1.0) / fstacks) * sin(pi * (j + 1.0) / fslices))));
            // Bottom right
            sphereVertices.push_back(vec3(
                                          radius * -cos(2.0f * pi * (i + 1.0) / fstacks) * sin(pi * j / fslices),
                                          radius * -cos(pi * j / fslices),
                                          radius * sin(2.0f * pi * (i + 1.0) / fstacks) * sin(pi * j / fslices)));
            sphereNormals.push_back(glm::normalize(vec3(
                                                        -cos(2.0f * pi * (i + 1.0) / fstacks) * sin(pi * j / fslices),
                                                        -cos(pi * j / fslices),
                                                        sin(2.0f * pi * (i + 1.0) / fstacks) * sin(pi * j / fslices))));
            
            // Need to repeat 2 of the vertices since we can only draw triangles. Eliminates the confusion
            // of array indices.
            // Top left
            sphereVertices.push_back(vec3(
                                          radius * -cos(2.0f * pi * i / fstacks) * sin(pi * (j + 1.0f) / fslices),
                                          radius * -cos(pi * (j + 1.0f) / fslices),
                                          radius * sin(2.0f * pi * i / fstacks) * sin(pi * (j + 1.0f) / fslices)));
            sphereNormals.push_back(glm::normalize(vec3(
                                                        -cos(2.0f * pi * i / fstacks) * sin(pi * (j + 1.0f) / fslices),
                                                        -cos(pi * (j + 1.0f) / fslices),
                                                        sin(2.0f * pi * i / fstacks) * sin(pi * (j + 1.0f) / fslices))));
            // Bottom right
            sphereVertices.push_back(vec3(
                                          radius * -cos(2.0f * pi * (i + 1.0) / fstacks) * sin(pi * j / fslices),
                                          radius * -cos(pi * j / fslices),
                                          radius * sin(2.0f * pi * (i + 1.0) / fstacks) * sin(pi * j / fslices)));
            sphereNormals.push_back(glm::normalize(vec3(
                                                        -cos(2.0f * pi * (i + 1.0) / fstacks) * sin(pi * j / fslices),
                                                        -cos(pi * j / fslices),
                                                        sin(2.0f * pi * (i + 1.0) / fstacks) * sin(pi * j / fslices))));
            // Bottom left
            sphereVertices.push_back(vec3(
                                          radius * -cos(2.0f * pi * i / fstacks) * sin(pi * j / fslices),
                                          radius * -cos(pi * j / fslices),
                                          radius * sin(2.0f * pi * i / fstacks) * sin(pi * j / fslices)));
            sphereNormals.push_back(glm::normalize(vec3(
                                                        -cos(2.0f * pi * i / fstacks) * sin(pi * j / fslices),
                                                        -cos(pi * j / fslices),
                                                        sin(2.0f * pi * i / fstacks) * sin(pi * j / fslices))));
            
        }
    }
    // Now bind this new vertex data
    glBindVertexArray(VAOs);
    
    // Bind vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBOs);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * sphereVertices.size(), &sphereVertices[0], GL_STATIC_DRAW);
    
    // Bind normals
    glBindBuffer(GL_ARRAY_BUFFER, NBOs);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * sphereNormals.size(), &sphereNormals[0], GL_STATIC_DRAW);
    
    // Done updating the buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    //    glUniformMatrix4fv(modelviewPos, 1, GL_FALSE, &modelview[0][0]);
    //    // Draw the sphere regardless of whether it was previously updated or not
    //    glBindVertexArray(VAOs[SPHERE]);
    //    glDrawArrays(GL_TRIANGLES, 0, sphereVertices.size());
    //    glBindVertexArray(0);
    //
}

Sphere::~Sphere()
{
    // Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a
    // large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
    
}

void Sphere::draw(GLuint shaderProgram, glm::mat4 V, glm::mat4 P)
{
    
    glm::mat4 mvp = P * V  * toWorld;
    GLuint mvpUniform = glGetUniformLocation(shaderProgram, "MVP");
    GLuint modelUniform = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(modelUniform, 1, GL_FALSE, &toWorld[0][0]);
    
    
    glBindVertexArray(VAOs);
    glDrawArrays(GL_TRIANGLES, 0, sphereVertices.size());
    glBindVertexArray(0);
    
}

void Sphere::reset(vec3 maxPoint){
    toWorld = mat4(1.0f);
    toWorld = translate(mat4(1.0f), vec3(maxPoint.x,maxPoint.y,maxPoint.z)) * toWorld;
    position = maxPoint;
}

void Sphere::moveToPos(vec3 position){
    toWorld = mat4(1.0f);
    toWorld = translate(mat4(1.0f), vec3(position.x,position.y,position.z)) * toWorld;
    this->position = position;
}

void Sphere::move(vec3 direction){
    //toWorld = mat4(1.0f);
    toWorld = translate(mat4(1.0f), vec3(direction.x,direction.y,direction.z)) * toWorld;
    this->position = position + direction;
}



vec3 Sphere::getPosition(){
    return position;
}



