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
	void draw();
	
	float rotSpeed;
	ofVec2f rot_axis;
	ofVec2f pos;
	float size;
	float active_size;
	int id;
	
	dsUser * pairedUser;
	bool isActive;
	bool isCovered;
	
	ofColor tc;
	vector<star*>* activeStarList;
	
	
};


#endif