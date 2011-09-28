/*
 *  strobingRings.h
 *  dsGraphics
 *
 *  Created by Simon Katan on 27/09/2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef STROBING_RINGS
#define STROBING_RINGS

#include "activeStarBase.h";

class strobingRings : public activeStarBase{
	
public:
	
	strobingRings(){
		
		starName = "strobingRings";
		newEvent = false;
		negMin = 40; negMax = 80;
		posMin = 40; posMax = 80;
		innerRing = 0.25;
		outerRing = 0.9;
		speed = 3;
		
	};
	
	int numpoints;
	float innerRing;
	float outerRing;
	ofVec2f offset;
	int count, speed;
	
	void update(float t_size, ofVec2f t_pos){
		
		if(newEvent){
			
			newEvent = true;
			speed = (eventPolarity > 0) ?  1 : ofRandom(6,10);
			
		}else if(eventTime == 0){
			
			speed = 3;
			
		}
		
		
		if(count == 0){
			
			innerRing = ofRandom(0.15,0.9);
			offset.set(ofRandom(-1,1),ofRandom(-1,1));
			
		}
		
		size = t_size;
		pos = t_pos;
		
		count =  (count + 1)%speed;
		numpoints = min(3000,(int)size * 10);
		
	}
	
	void draw(){
		
		ofSetColor(255);
		glPushMatrix();
		glTranslatef(pos.x,pos.y ,0);
		
		
		glBegin(GL_POINTS);
		for(int i = 0; i < numpoints; i ++){
			
			float d;
			float r = ofRandom(0,1);
			ofVec2f p;
			
			if(r < 0.25){
				d = min(ofRandom(0,innerRing * size * 1.3),ofRandom(0,innerRing * size * 1.3));
				p.set(0,d);
				p.rotate(ofRandom(0,360),ofVec2f(0,0));
			}else{
				d = max(ofRandom(innerRing * 0.9 * size,innerRing * size),ofRandom(innerRing * 0.9 * size,innerRing *size));
				p.set(0,d);
				p.rotate(ofRandom(0,360),ofVec2f(0,0));
				p += offset * size/5;
			}

			glVertex2f(p.x,p.y);
		}
		glEnd();
		
		
		glPopMatrix();
		
	};
	
	
};

#endif
