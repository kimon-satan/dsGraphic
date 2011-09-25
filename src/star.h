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
#include "starIncludes.h"
#include "activeStarBase.h"

class star{

public:

	star();
	~star();
	
	void update();
	void findConflicts();
	void findIsCovered();
	void drawBG(bool isAlpha);
	void drawActiveAlgorithm(bool isAlpha);
	void twinkle(int t_count = 0);
	void assignAlgorithm();
	
	float rotSpeed;
	ofVec2f pos;
	float size, active_size, max_size, worldCircum;
	float intensity, alpha;
	int id, col;
	bool twinkling, onScreen, isUserMoving;
	
	dsUser * pairedUser;
	bool isActive, isCovered;
	float base, add ,inc, mul;
	int count;

	ofColor tc;
	vector<star*>* activeStarList;
	activeStarBase  * activeStar;
	

	
	
};


#endif