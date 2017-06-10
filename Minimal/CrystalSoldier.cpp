//
//  16Terrain.cpp
//  West World
//
//  Created by Jiasheng Zhu on 3/19/17.
//  Copyright © 2017 Jiasheng Zhu. All rights reserved.
//

#include "CrystalSoldier.h"

using namespace std;


CrystalSoldier::CrystalSoldier(vec3 pos)
{
    toWorld = glm::mat4(1.0f);
	initPos = pos;
	displaceTrans = glm::translate(glm::mat4(1.0f), pos);
	headRotate = mat4(1.0f);
	lArmRotate = mat4(1.0f);
	rArmRotate = mat4(1.0f);
	rArm = new CrystalArm();
	lArm = new CrystalArm();
	body = new CrystalBody();
	initLPoint = vec3(-0.2f, -0.25f, -0.1f);
	initRPoint = vec3(0.2f, -0.25f, -0.1f);
	initLArm = vec3(-0.2f, 1.0f, 0.0f) -initLPoint;
	initRArm = vec3(0.2f, 1.0f, 0.0f) -initRPoint;
	
	rightArmTrans = glm::translate(glm::mat4(1.0f),initRPoint);
	leftArmTrans = glm::translate(glm::mat4(1.0f), initLPoint);

    
    
}

CrystalSoldier::~CrystalSoldier()
{
 
}

void CrystalSoldier::draw(GLuint shaderProgram, glm::mat4 V, glm::mat4 P)
{
	body->draw(shaderProgram, V * displaceTrans * headRotate, P);
	//rArm->rotateArm(rightArmTrans, headRotate, rArmRotate, rScale);
	//rArm->draw(shaderProgram, V  * displaceTrans , P);
	//lArm->rotateArm(leftArmTrans, headRotate, lArmRotate, lScale);
	//lArm->draw(shaderProgram, V  * displaceTrans , P);
}

void CrystalSoldier::moveSoldier(vec3 displace) 
{
	displaceTrans = glm::translate(glm::mat4(1.0f), initPos + displace);
	
}

void CrystalSoldier::rotateSoldier(mat4 yRotate) {
	headRotate = yRotate;

}

void CrystalSoldier::rotateArm(vec3 leftEnd, vec3 rightEnd) {
	//cout << "x: " << leftEnd.x << " y: " << leftEnd.y << " z: " <<  leftEnd.z << endl;
	vec3 curLArm = leftEnd - initLPoint;
	vec3 curRArm = rightEnd - initRPoint;
	float lRadian = acos(dot(curLArm, initLArm));
	float rRadian = acos(dot(curRArm, initRArm));
	vec3 lAxis = normalize( cross(curLArm,initLArm));
	vec3 rAxis = normalize(cross(curRArm, initRArm));
	lArmRotate = rotate(mat4(1.0f), lRadian, lAxis);
	rArmRotate = rotate(mat4(1.0f), rRadian, rAxis);
	//cout << length(curLArm) << endl;
	lScale = scale(mat4(1.0f), vec3(1.0f,pow(length(curLArm),3) ,1.0f));
	rScale = scale(mat4(1.0f), vec3(1.0f, pow(length(curRArm),3), 1.0f));
}



void CrystalSoldier::update()
{
}

