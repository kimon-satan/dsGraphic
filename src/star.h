/*
 *  star.h
 *  DS_Graphics
 *
 *  Created by Simon Katan on 14/09/2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef STAR
#define STAR

#include "ofMain.h" 
#include "dsUser.h"



class star{

public:

	star();
	~star();
	
	void update();
	void findConflicts();
	void findIsCovered();
	void drawBG(bool isAlpha);
	void drawActiveAlgorithm(bool isAlpha);
	void twinkle();
	
	float rotSpeed;
	ofVec2f rot_axis;
	ofVec2f pos;
	float size;
	float active_size;
	float intensity;
	float alpha;
	float worldCircum;
	int id;
	bool twinkling;
	
	dsUser * pairedUser;
	bool isActive;
	bool isCovered;
	float base;
	float add;
	float inc;

	ofColor tc;
	vector<star*>* activeStarList;
	
	
};


#endif