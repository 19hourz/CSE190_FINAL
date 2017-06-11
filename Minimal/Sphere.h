//
//  Sphere.hpp
//  CSE167_PA3
//
//  Created by Ryan Li on 10/29/16.
//  Copyright © 2016 Ryan Li. All rights reserved.
//
#ifndef SPHERE_H
#define SPHERE_H

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
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>

using namespace glm;
using namespace std;

class Sphere
{
private:
    int lats, longs;
    bool isInited;
    vec3 position;


public:
    Sphere(float,int,int,glm::vec3);
    ~Sphere();
    
    glm::mat4 toWorld;
    
    void initSphere();
    void draw(GLuint shaderProgram, glm::mat4 V, glm::mat4 P);
    void reset(vec3);
    void moveToPos(vec3);
    void move(vec3);

    vec3 getPosition();
    
    GLuint VAOs, VBOs, NBOs;
    vector<vec3> sphereVertices;
    vector<vec3> sphereNormals;

};

#endif // SPHERE_H
