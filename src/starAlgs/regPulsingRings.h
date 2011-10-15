

/*
 *  pulsatingRings.h
 *  dsGraphics
 *
 *  Created by Simon Katan on 27/09/2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */



#ifndef REG_PULSING_RINGS
#define REG_PULSING_RINGS

#include "activeStarBase.h";

class regPulsingRings : public activeStarBase{
	
public:
	
	regPulsingRings(){
		
		starName = "regPulsingRings";
		newEvent = false;
		negMin = 40; negMax = 120;
		posMin = 120; posMax = 180;
		numrings = 9;
		
		for(int i = 0; i < numrings; i++){
			
			rings.push_back(0);
			offsets.push_back(i * 10);
			origs.push_back(0);
			
		}
		
		
	};
	
	int numpoints;
	int numrings;
	vector <float> rings;
	vector <float> offsets;
	vector <float> origs;
	float mul;
	float t_point;
	int eventLength;
	
	
	void update(float t_size, ofVec2f t_pos){
		
		if(newEvent){
			
			if(ofRandom(0,1) > 0.5){
				
				newEvent = false; //override to reduce number of events
				eventTime = 0;
			}
		}
		
		
		if(newEvent){
			
			eventLength = eventTime;
			mul = 180.0f/eventTime;
			t_point = eventTime * 0.2f;
			for(int i = 0; i < numrings; i ++)origs[i] = rings[i];
			
			
		}else if(eventTime == 0){
			
			int deg = ofGetFrameNum();
			
			for(int i = 0; i < numrings; i ++)
			{
				float f = (float)deg * 1 + offsets[i];
				f = fmodf(f, 90);
				rings[i] = abs(
							   sin(
								   ofDegToRad(f)
								   )
							   );
			}
			
		}
		
		if(eventTime > 0){
			
			float f, deg;
			
			if(eventPolarity == 1){
				
				//remember this for skewing sines next time !
				deg = (eventTime > t_point) ?  -(eventLength - eventTime) * mul * 5/8 :  180 + eventTime * mul * 5/2;
				f = sin(ofDegToRad(deg));
				float f1 = sin(ofDegToRad(180 + eventTime * mul));
				
			}else{
				
				deg = eventTime * mul;
				f = 0.7 * sin(ofDegToRad(deg));
				
			}
			
			for(int i = 0; i < numrings; i ++){
				rings[i] = origs[i] + origs[i] * f;
			}
			
		}
		
		size = t_size;
		pos = t_pos;
		numpoints = min(5000,(int)size * 13);
		
	}
	
	void draw(){
		
		ofSetColor(255);
		glPushMatrix();
		glTranslatef(pos.x,pos.y ,0);
		
		
		glBegin(GL_POINTS);
		for(int i = 0; i < numpoints; i ++){
			
			bool skip = false;
			float d;
			float r = ofRandom(0,1);
			
			if(r < 0.05){
				d = min(ofRandom(0,size * 0.3),ofRandom(0,size * 0.3));
			}else{
				d = max(ofRandom(rings[i%numrings] * 0.95 * size,rings[i%numrings] * size),ofRandom(rings[i%numrings] * 0.95 * size,rings[i%numrings] *size));
			}
			
			if(!skip){
			ofVec2f p(0,d);
			p.rotate(ofRandom(0,360),ofVec2f(0,0));
			glVertex2f(p.x,p.y);
			}
		}
		glEnd();
		
		
		glPopMatrix();
		
	};
	
	
};

#endif
