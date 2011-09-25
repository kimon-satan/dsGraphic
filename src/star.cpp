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
	
	rotSpeed = -0.15;
	isActive = false;
	isCovered = false;
	isUserMoving = false;
	max_size = 300;
	assignAlgorithm();
	pairedUser = NULL;
	float sizeArray[6] = {0.5,1,2.75,3.75,5.75,7.75};
	float probArray[6] = {0.16,0.16,0.3,0.24,0.1,0.06};
	float cum = 0;
	float rnd = ofRandom(0,1);
	int index = 5;
	for(int i = 0; i < 6; i++){
		cum += probArray[i];
		if(rnd < cum){index = i;break;}
	}
	
	size = sizeArray[index];
	active_size = size;
	
	tc.r = 255;
	tc.g = 255; 
	tc.b = 255;
	tc.a = 255; 
	
	if(size < 2){
		alpha = 150;
		intensity = (ofRandomf()> 0.7)? min(ofRandom(0.5,0.7),ofRandom(0.5,0.7)) : 0;
	}else if(size < 5){
		alpha = 90;
		intensity = 0;//min(ofRandom(0.05,0.2), ofRandom(0.05,0.2));
	}else{
		alpha = 50;
		intensity = 0;
	}
	
	twinkling = false;
	base = intensity;
	
}

void star::update(){
	
	
	if(!isActive){
		pos.x += rotSpeed;
		if(pos.x > worldCircum/2){
			pos.x = -worldCircum/2;
		}else if(pos.x < -worldCircum/2){
			pos.x = worldCircum/2;
		}
		
		(abs(pos.x) > ofGetScreenWidth()/2) ? onScreen = false : onScreen = true;
		if(twinkling)twinkle();
		findIsCovered();
		if(pairedUser)pairedUser = NULL;
		
	}else if(!pairedUser->isActive){
		
		//pairedUser = NULL; //we still need the reference for osc messaging 
		isActive = false;
		active_size = size;
		intensity = base;
		//findIsCovered(); //no need as it won't be covered as just disappeared
		
	}else{
		
		findConflicts();
		
		if(active_size > 25){
		
			ofVec2f dir = pairedUser->pos - pos;
			dir.normalize();
			pos += dir * 0.3; //attractor for star
			
		}
		
		if(isUserMoving != pairedUser->isMoving){
			
			activeStar->newEvent = true;
			activeStar->eventTime = ofRandom(40,80);
			activeStar->eventPolarity = (pairedUser->isMoving) ? 1 : - 1;
			isUserMoving = pairedUser->isMoving;
			
		}else if(activeStar->eventTime > 0){
			activeStar->eventTime -= 1;
			activeStar->newEvent = false;
		}
		
		activeStar->update(active_size, pos); 
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
	
	if(!growBlocked){active_size = min(max_size,active_size + 0.05f);}
	
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

void star::twinkle(int t_count){
	 
	mul = 1 - base;
	
	if(!twinkling){
		twinkling = true;
		add = min(ofRandom(0.02,0.04), ofRandom(0.02,0.04));
		inc = 0.01;
		count = t_count;
	}
	
	
	if(add > 0){
		inc += add;
		intensity = base + mul * pow(inc,2);
		if(inc > 1){
			add *= -1;	
		}
	}else if(count > 0){
		count -= 1;
	}else{
		inc += add;
		intensity = max(base,(base + mul * pow(inc,2)));
		if(inc < 0){
			twinkling = false;
			intensity = base;
		}
	}
	
		
}

void star::drawBG(bool isAlpha){
	
	if(!isActive && onScreen && intensity > 0){
		
		ofFill();
		(isAlpha) ? ofSetColor(tc.r * intensity,tc.b * intensity, tc.g * intensity, alpha): ofSetColor(tc.r * intensity,tc.g * intensity,tc.b * intensity);
		
		if(size > 1){
		ofCircle(pos.x,pos.y, size);
		}else if(size == 1){
			glBegin(GL_POINTS);
			glVertex2f(pos.x, pos.y);
			glVertex2f(pos.x -1, pos.y);
			glVertex2f(pos.x, pos.y -1 );
			glVertex2f(pos.x - 1, pos.y - 1);
			glEnd();
		}else{ 
			glBegin(GL_POINTS);
				glVertex2f(pos.x, pos.y);
			glEnd();
		}
	}
	
}

void star::drawActiveAlgorithm(bool isAlpha){
	
	if(isActive){
		activeStar->draw(isAlpha);
	}	
}

void star::assignAlgorithm(){

	activeStar = new randomRect();
	activeStar->max_size = max_size;
	
}


star::~star(){
	
	//delete activeStar;
	
}