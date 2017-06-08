//
//  Terrain16.h
//  West World
//
//  Created by Jiasheng Zhu on 3/19/17.
//  Copyright © 2017 Jiasheng Zhu. All rights reserved.
//

#ifndef _CrystalBody_h
#define _CrystalBody_h

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CrystalBody
{
public:
    CrystalBody();
    ~CrystalBody();
    
    glm::mat4 toWorld;
    
    void draw(GLuint, glm::mat4, glm::mat4);
    void update();
    

    GLuint VBO, VAO, EBO, NAO;
    GLuint uProjection, uModelview;
    const static GLuint numberSideBlocks = 160;
    const static GLuint numberSideVertices = 170;
    const static GLuint numberVertices = 4;
    const static GLuint numberIndices = 4;
};
#endif
