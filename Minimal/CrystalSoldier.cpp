//
//  16Terrain.cpp
//  West World
//
//  Created by Jiasheng Zhu on 3/19/17.
//  Copyright © 2017 Jiasheng Zhu. All rights reserved.
//

#include "CrystalSoldier.h"


CrystalSoldier::CrystalSoldier(vec3 pos)
{
    toWorld = glm::mat4(1.0f);
	initPos = pos;
	displaceTrans = glm::translate(glm::mat4(1.0f), pos);
	rotateTrans = mat4(1.0f);
	rArm = new CrystalArm();
	lArm = new CrystalArm();
	body = new CrystalBody();

	leftArmTrans = glm::translate(glm::mat4(1.0f), glm::vec3(-0.3, -0.25, 0.0));
	rightArmTrans = glm::translate(glm::mat4(1.0f), glm::vec3(0.3, -0.25, 0.0));

    
    
}

CrystalSoldier::~CrystalSoldier()
{
 
}

void CrystalSoldier::draw(GLuint shaderProgram, glm::mat4 V, glm::mat4 P)
{
	body->draw(shaderProgram, V * displaceTrans * rotateTrans, P);
	rArm->rotateArm(rightArmTrans, rotateTrans);
	rArm->draw(shaderProgram, V  * displaceTrans , P);
	lArm->rotateArm(leftArmTrans, rotateTrans);
	lArm->draw(shaderProgram, V  * displaceTrans , P);
}

void CrystalSoldier::moveSoldier(vec3 displace) 
{
	displaceTrans = glm::translate(glm::mat4(1.0f), initPos + displace);
	
}

void CrystalSoldier::rotateSoldier(mat4 yRotate) {
	rotateTrans = yRotate;

}

void CrystalSoldier::rotateArm(vec3 leftEnd, vec3 rightEnd) {




}



void CrystalSoldier::update()
{
}

