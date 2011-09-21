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
	
	rotSpeed = 0.15;
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
		intensity = min(ofRandom(0.1,0.3), ofRandom(0.1,0.2));
	}else if(size < 5){
		alpha = 90;
		intensity = min(ofRandom(0.1,0.3), ofRandom(0.1,0.2));
	}else{
		alpha = 50;
		intensity = min(ofRandom(0.1,0.3), ofRandom(0.1,0.2));
	}
	
	twinkling = false;
	base = intensity;
	
	
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

void star::twinkle(){
	
	static int count;
	
	if(!twinkling){
		twinkling = true;
		add = min(ofRandom(0.005,0.04), ofRandom(0.005,0.04));
		inc = 0.01;
		count = 0;
	}
		
	inc += add;
	float mul = 1 - base; 
	
	if(add > 0){
		intensity = base + mul * pow(inc,2);
	}else{
		intensity = max(base,(base + mul * pow(inc,2)));
	}
	
		if(inc > 1){
			add *= -1;
		}else if(inc < 0){
			twinkling = false;
			intensity = base;
		}
		
		
}

void star::drawBG(bool isAlpha){
	
	if(!isActive){
		
		ofFill();
		(isAlpha) ? ofSetColor(tc.r * intensity,tc.b * intensity, tc.g * intensity, alpha): ofSetColor(tc.r * intensity,tc.g * intensity,tc.b * intensity);
		ofCircle(pos.x,pos.y, size);

	}
	
}

void star::drawActiveAlgorithm(bool isAlpha){
	
	if(isActive){
		
		static ofRectangle rect;
		static float rot = 0;
		static float rot_add = 0;
		static ofColor col = 0;
		
		if(!isAlpha){ // set statics only on 1st pass
			rot_add =ofRandom(-20,20);
			rect.set(ofRandom(-20,20), ofRandom(-20,20), ofRandom(50,200), ofRandom(50,200));
			rot += 0.1;
			col.set(ofRandom(200,255), ofRandom(200,255), ofRandom(200,255), 255);
		}

		glPushMatrix();
		glTranslatef(pos.x,pos.y ,0);
		glScalef(active_size/100.0f,active_size/100.0f, 1);
		
			glPushMatrix();
				ofSetRectMode(OF_RECTMODE_CENTER);
				glRotated(rot + rot_add, 0, 0, 1);
				if(!isAlpha){
					ofSetColor(0, 0, 0);
					ofRect(rect.x,rect.y, rect.width + 10, rect.height + 10); // a slightly larger rect of black for blur
					ofSetColor(col);
				}else{
					ofSetColor(col.r, col.g, col.b, 100);
				}
				ofRect(rect.x,rect.y,rect.width,rect.height);
				ofSetRectMode(OF_RECTMODE_CORNER);
			glPopMatrix();
		
		glPopMatrix();
	}
	
}

star::~star(){
	
	
}