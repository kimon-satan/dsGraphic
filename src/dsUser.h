/*
 *  dsUser.h
 *  DS_Graphics
 *
 *  Created by Simon Katan on 14/09/2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef DS_USER
#define DS_USER

class dsUser{
	
public:
	
	dsUser(){
		
		reset();
		isActive = true;
	
	}
	
	void reset(){
		
		pos.set(0,0);
		isActive = true;
		isPaired = false;	
		ghostCount = 0;
		isMoving = false;
		stillCount = 0;
	}
	
	void updateHistory(){
		
		if(!isMoving){ 
			stillCount += 1;
		}else{ stillCount = 0;}
	}
	
	int ghostCount;
	int stillCount;
	ofVec2f pos;
	bool isActive, isPaired, isMoving ,isStill, isFake;
	int starId;
	int id;
	
};


#endif