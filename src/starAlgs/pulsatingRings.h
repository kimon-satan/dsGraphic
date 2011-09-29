/*
 *  pulsatingRings.h
 *  dsGraphics
 *
 *  Created by Simon Katan on 27/09/2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */



#ifndef PULSATING_RINGS
#define PULSATING_RINGS

#include "activeStarBase.h";

class pulsatingRings : public activeStarBase{
	
public:
	
	pulsatingRings(){
		
		starName = "pulsatingRings";
		newEvent = false;
		negMin = 40; negMax = 80;
		posMin = 40; posMax = 80;
		numrings = 10;
		
		for(int i = 0; i < 10; i++){
		
			rings.push_back(0);
			rates.push_back((float)1.0f/(1+i));
			offsets.push_back(i * 20);
			
		
		}
		
		
	};
	
	int numpoints;
	int numrings;
	vector <float> rings;
	vector <float> rates;
	vector <float> offsets;
	
	void update(float t_size, ofVec2f t_pos){
		
		if(newEvent){
			
			newEvent = true;
			
		}else if(eventTime == 0){
			
			int deg = ofGetFrameNum();
			
			for(int i = 0; i < numrings; i ++)
			{
				float f = (float)deg * rates[i] + offsets[i];
				f = fmodf(f, 180);
				rings[i] = abs(
					sin(
						ofDegToRad(f)
						)
				);
			}

		}
		
		size = t_size;
		pos = t_pos;
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
			
			if(r < 0.1){
				d = min(ofRandom(0,size/6),ofRandom(0,size/6));
			}else if(r < 0.95){
				d = max(ofRandom(rings[i%numrings] * 0.95 * size,rings[i%numrings] * size),ofRandom(rings[i%numrings] * 0.95 * size,rings[i%numrings] *size));
			}else{
				d = ofRandom(size * 0.9, size);
			}
			
			ofVec2f p(0,d);
			p.rotate(ofRandom(0,360),ofVec2f(0,0));
			glVertex2f(p.x,p.y);
		}
		glEnd();
		
		
		glPopMatrix();
		
	};
	
	
};

#endif
