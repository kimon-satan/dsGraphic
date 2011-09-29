/*
 *  strobingLines.h
 *  dsGraphics
 *
 *  Created by Simon Katan on 28/09/2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef STROBING_LINES
#define STROBING_LINES

#include "activeStarBase.h";

class strobingLines : public activeStarBase{
	
public:
	
	int numpoints, numLines;
	vector<ofVec2f> line;
	vector<float> rot;
	bool blkOut;
	
	strobingLines(){
		
		starName = "strobingLines";
		newEvent = false;
		negMin = 80; negMax = 160;
		posMin = 30; posMax = 60;
		numLines = 6;
		
		blkOut = false;
		
		for(int i = 0; i < numLines; i ++){
			line.push_back(ofVec2f(0,0));
			rot.push_back(0);
		}
		
	};
	
	void update(float t_size, ofVec2f t_pos){
		
		if(newEvent){
			
			newEvent = true;
			
		}else if(eventTime == 0){
			
			blkOut = false;
			newLines();

		}
		
		if(eventTime > 0){
			
			if(eventPolarity == -1){
				newLines();
				blkOut = (ofRandom(0,1) > 0.3) ? true: false;
			}else{
				blkOut = (ofRandom(0,1) > 0.15) ? true: false;
			}
		
		}
	
		size = t_size;
		pos = t_pos;
		
		numpoints = min(3000,(int)size * 10);
		
	};
	
	void newLines(){
		
		for(int i = 0; i < numLines; i ++){
			line[i].y = ofRandom(0,0.8);
			line[i].x = sqrt(1 - pow(line[i].y,2));
			line[i].x *= ofRandom(0.8,1.2); // a bit of flare
			rot[i] = ofRandom(0,360);
		}
	
	};
	
	void draw(){
		
		if(!blkOut){
			ofSetColor(255);
			glPushMatrix();
			glTranslatef(pos.x,pos.y ,0);
			
			
			glBegin(GL_POINTS);
			for(int i = 0; i < numpoints; i ++){
				
				ofVec2f p;
			
				p.x = ofRandom(-line[i%numLines].x * size * ofRandom(1,1.2),line[i%numLines].x * size * ofRandom(1,1.2));
				p.y = ofRandom(line[i%numLines].y * size ,line[i%numLines].y * size * 0.9);
				p.rotate(rot[i%numLines], ofVec2f(0,0));

				
				glVertex2f(p.x,p.y);
			}
			glEnd();
			
			
			glPopMatrix();
		}
		
	};
	
	
};

#endif
