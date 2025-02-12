//
//  Terrain16.h
//  West World
//
//  Created by Jiasheng Zhu on 3/19/17.
//  Copyright © 2017 Jiasheng Zhu. All rights reserved.
//

#ifndef _CrystalSoldier_h
#define _CrystalSoldier_h

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
#include <iostream>
#include "CrystalBody.h"
#include "CrystalArm.h"

using namespace glm;

class CrystalSoldier
{

public:
	
	glm::mat4 toWorld;
	CrystalArm *rArm, *lArm;
	CrystalBody *body;
	mat4 displaceTrans;
	mat4 headRotate, lArmRotate, rArmRotate, armDirRotate;
	vec3 initLPoint, initRPoint, initLArm, initRArm;
	vec4 rArmTop, lArmTop;

	glm::mat4 leftArmTrans, rightArmTrans;


    CrystalSoldier();
    ~CrystalSoldier(); 
    void draw(GLuint, glm::mat4, glm::mat4, bool);
	void moveSoldier(vec3 initPos,vec3 displace);
	void rotateSoldier(float yAngle);
	void rotateArm(vec3 leftEnd, vec3 rightEnd, GLfloat headAngle);
	vec3 getRArmTopPoint();
	vec3 getLArmTopPoint();
   
    

};
#endif
