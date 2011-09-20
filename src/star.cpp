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
	
	rotSpeed = 0;//0.15;
	rot_axis.set(0,0);
	isActive = false;
	isCovered = false;
	pairedUser = NULL;
	size = min(ofRandom(1,5), ofRandom(1,5));
	active_size = size;
	
	tc.r = max(ofRandom(200,255),ofRandom(200,255));
	tc.g = max(ofRandom(200,255),ofRandom(200,255));
	tc.b = max(ofRandom(200,255),ofRandom(200,255));
	tc.a = min(ofRandom(0,200), ofRandom(0,200));
	twinkling = false;
	base = tc.a;

	
}

void star::update(){
	
	if(!isActive){
		pos.x += rotSpeed;
		if(pos.x > worldCircum/2){
			pos.x = -worldCircum/2;
		}
		
		if(twinkling)twinkle();
		findIsCovered();
		
	}else if(!pairedUser->isActive){
		
		isActive = false;
		pairedUser = NULL;
		active_size = size;

		tc.a = base;
		//findIsCovered(); //no need as it won't be covered as just disappeared
		
	}else{
		
		tc.a = 100;
		findConflicts();
	}
	
}

void star::findConflicts(){
	
	bool growBlocked = false;
	
	for(int i = 0; i < activeStarList->size(); i++){
		
		star * s = activeStarList->at(i);
		
		if(s->id != id){
		
			if(pos.distance(s->pos) < s->active_size + active_size){
				
				if(s->active_size > active_size){
						
					growBlocked = true;
					
				}else{
					
					if(s->active_size > 5){ s->active_size -= 0.05;}else{
					
						growBlocked = true;
						
					}
				}
				
				
			}
		}
	}
	
	if(!growBlocked)active_size += 0.05;
	
}

void star::findIsCovered(){
	
	isCovered = false;
	
	for(int i = 0; i < activeStarList->size(); i++){
		
		star * s = activeStarList->at(i);
		if(pos.distance(s->pos) < s->active_size){
			
			isCovered = true;
			
		}else if(!isCovered){ //check not already covered
			
			isCovered = false;
		}
	}
}

void star::twinkle(){
	
	if(!twinkling){
		twinkling = true;
		add = ofRandom(0.01,0.02);
		inc = 0.01;
	}
	
	float mul = 256 - base;
		
		inc += add;
	if(add > 0){
		tc.a = base + mul * pow(inc,2);
	}else{
		tc.a = max(base,(base + mul * sqrt(inc)));
	}
	
		if(inc > 1){
			add *= -1;
		}else if(inc < 0){
			twinkling = false;
		}
		

	
		
}

void star::draw(){
	
	ofEnableAlphaBlending();
	if(!isActive){
		
		ofFill();
		ofSetColor(tc);
		ofCircle(pos.x,pos.y, size);
		
	}else{
		//ofNoFill();
		//ofSetColor(tc);
		//ofCircle(pos.x,pos.y, active_size);
		drawActiveAlgorithm();
	}
	
	ofDisableAlphaBlending();
	ofSetColor(255);
}

void star::drawActiveAlgorithm(){
	
	glPushMatrix();
	glTranslatef(pos.x,pos.y ,0);
	glScalef(active_size/100.0f,active_size/100.0f, 1);
	
	glPushMatrix();
		ofSetRectMode(OF_RECTMODE_CENTER);
		static float rot = 0;
		rot += 0.1;
		glRotated(rot + ofRandom(-20,20), 0, 0, 1);
		ofSetColor(ofRandom(200,255), ofRandom(200,255), ofRandom(200,255));
		ofRect(ofRandom(-10,10), ofRandom(-10,10), ofRandom(50,200), ofRandom(50,200));
		ofSetRectMode(OF_RECTMODE_CORNER);
	glPopMatrix();
	
	glPopMatrix();
}

star::~star(){
	
	
}