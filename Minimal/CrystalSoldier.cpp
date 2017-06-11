//
//  16Terrain.cpp
//  West World
//
//  Created by Jiasheng Zhu on 3/19/17.
//  Copyright © 2017 Jiasheng Zhu. All rights reserved.
//

#include "CrystalSoldier.h"

using namespace std;


CrystalSoldier::CrystalSoldier()
{
    toWorld = glm::mat4(1.0f);
	displaceTrans = glm::mat4(1.0f);
	headRotate = mat4(1.0f);
	armDirRotate = mat4(1.0f);
	lArmRotate = mat4(1.0f);
	rArmRotate = mat4(1.0f);
	rArm = new CrystalArm();
	lArm = new CrystalArm();
	body = new CrystalBody();
	initLPoint = vec3(-0.1f, -0.25f, -0.3f);
	initRPoint = vec3(0.1f, -0.25f, -0.3f);
	//initLArm = initLPoint - vec3(-0.2f, 1.0f, -0.1f);
	//initRArm = initRPoint - vec3(0.2f, 1.0f, -0.1f);
	initLArm = vec3(0.0f, 1.0f, 0.0f);
	initRArm = vec3(0.0f, 1.0f, 0.0f);
	
	rightArmTrans = glm::translate(glm::mat4(1.0f),initRPoint);
	leftArmTrans = glm::translate(glm::mat4(1.0f), initLPoint); 
}

CrystalSoldier::~CrystalSoldier()
{
 
}

void CrystalSoldier::draw(GLuint shaderProgram, glm::mat4 V, glm::mat4 P)
{	
	body->rotateBody(headRotate);
	body->draw(shaderProgram, V * displaceTrans, P);
	rArm->rotateArm(rightArmTrans, headRotate, rArmRotate, armDirRotate);
	rArm->draw(shaderProgram, V  * displaceTrans , P);
	lArm->rotateArm(leftArmTrans, headRotate, lArmRotate, armDirRotate);
	lArm->draw(shaderProgram, V  * displaceTrans , P);
}

void CrystalSoldier::moveSoldier(vec3 initPos, vec3 displace) 
{
	displaceTrans = glm::translate(glm::mat4(1.0f), initPos + displace);
	
}

void CrystalSoldier::rotateSoldier(float yAngle) {
	headRotate = glm::rotate(mat4(1.0f), yAngle, vec3(0.0f, 1.0f, 0.0f));;
}

void CrystalSoldier::rotateArm(vec3 leftEnd, vec3 rightEnd, GLfloat headAngle) {
	//cout << "x: " << leftEnd.x << " y: " << leftEnd.y << " z: " <<  leftEnd.z << endl;
	//cout << "x: " << rightEnd.x << " y: " << rightEnd.y << " z: " << rightEnd.z << endl;
	//vec2 lTemp = cos(headAngle) * vec2(leftEnd.x, leftEnd.z);
	//vec3 curLArm = vec3(-lTemp.x, leftEnd.y, -lTemp.y);
	//vec2 rTemp = cos(headAngle) * vec2(rightEnd.x, rightEnd.z);
	//vec3 curRArm = vec3(-rTemp.x, rightEnd.y, -rTemp.y);
	vec3 curLArm = vec3(-leftEnd.x , leftEnd.y, -leftEnd.z );
	vec3 curRArm = vec3(-rightEnd.x, rightEnd.y, -rightEnd.z);
	float lRadian = acos(dot(curLArm, initLArm));
	float rRadian = acos(dot(curRArm, initRArm));
	vec3 lAxis = normalize( cross(curLArm,initLArm));
	vec3 rAxis = normalize(cross(curRArm, initRArm));
	lArmRotate = rotate(mat4(1.0f), lRadian, lAxis);
	rArmRotate = rotate(mat4(1.0f), rRadian, rAxis);
	
	armDirRotate = rotate(mat4(1.0f), -headAngle, vec3(0.0f,1.0f,0.0f));
	//cout << length(curLArm) << endl;
}



void CrystalSoldier::update()
{
}

