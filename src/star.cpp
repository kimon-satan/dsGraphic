/*
 *  star.cpp
 *  DS_Graphics
 *
 *  Created by Simon Katan on 14/09/2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "star.h"


star::star(){
	
	rotSpeed = 0.025;
	rot_axis.set(0,0);
	isActive = false;
	isCovered = false;
	pairedUser = NULL;
	size = ofRandom(1,2);
	active_size = size;
	
	tc.r = ofRandom(200,255);
	tc.g = ofRandom(200,255);
	tc.b = max(ofRandom(200,255),ofRandom(200,255));
	
}

void star::update(){
	
	if(!isActive){
		pos.rotate(rotSpeed,rot_axis);
	}else if(!pairedUser->isActive){
		isActive = false;
		pairedUser = NULL;
		active_size = size;
	}else{
		active_size *= 1.01;
	}
	
	for(int i = 0; i < activeStarList->size(); i++){
		
		star * s = activeStarList->at(i);
		if(pos.distance(s->pos) < s->active_size){
			
			isCovered = true;
			
		}else{
			
			isCovered = false;
		}
	}
	
}

void star::draw(){
	
	if(!isActive){
		
		if(!isCovered){
			ofFill();
			ofSetColor(tc);
			ofCircle(pos.x,pos.y, size);
		}
	}else{
		ofNoFill();
		ofSetColor(tc);
		ofCircle(pos.x,pos.y, active_size);
	}
	
	ofSetColor(255);
}


star::~star(){
	
	
}