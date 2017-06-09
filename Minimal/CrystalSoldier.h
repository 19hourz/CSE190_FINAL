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
	vec3 initPos;
	mat4 displaceTrans;
	mat4 headRotate, lArmRotate, rArmRotate, rScale, lScale;
	vec3 initLPoint, initRPoint, initLArm, initRArm;

	glm::mat4 leftArmTrans, rightArmTrans;


    CrystalSoldier(vec3 initPos);
    ~CrystalSoldier(); 
    void draw(GLuint, glm::mat4, glm::mat4);
	void moveSoldier(vec3 displace);
	void rotateSoldier(mat4 yRotate);
	void rotateArm(vec3 leftEnd, vec3 rightEnd);
    void update();
    

};
#endif
