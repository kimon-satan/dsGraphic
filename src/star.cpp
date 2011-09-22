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
	rot_axis.set(0,0);
	isActive = false;
	isCovered = false;
	pairedUser = NULL;
	size = min(ofRandom(1,7), min(ofRandom(1,7),ofRandom(1,7)));
	active_size = size;
	
	tc.r = 255;
	tc.g = 255; 
	tc.b = 255;
	tc.a = 255; 
	
	if(size < 2){
		alpha = 180;
		intensity = min(ofRandom(0.05,0.2), ofRandom(0.05,0.2));
	}else if(size < 5){
		alpha = 90;
		intensity = min(ofRandom(0.05,0.2), ofRandom(0.05,0.2));
	}else{
		alpha = 50;
		intensity = min(ofRandom(0.05,0.2), ofRandom(0.05,0.2));
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
		
	}else if(!pairedUser->isActive){
		
		isActive = false;
		pairedUser = NULL;
		active_size = size;

		intensity = base;
		//findIsCovered(); //no need as it won't be covered as just disappeared
		
	}else{
		
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
	
	if(!isActive && onScreen){
		
		ofFill();
		(isAlpha) ? ofSetColor(tc.r * intensity,tc.b * intensity, tc.g * intensity, alpha): ofSetColor(tc.r * intensity,tc.g * intensity,tc.b * intensity);
		ofCircle(pos.x,pos.y, size);

	}
	
}

void star::drawActiveAlgorithm(bool isAlpha){
	
	if(isActive){
		rA.draw(pos,isAlpha, active_size);
	}	
}

star::~star(){
	
	
}