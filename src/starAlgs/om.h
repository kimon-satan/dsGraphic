/*
 *  om.h
 *  dsGraphics
 *
 *  Created by Simon Katan on 02/10/2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef OM
#define OM

#include "activeStarBase.h";

class om : public activeStarBase{
	
public:
	
	om(){
		
		starName = "om";
		newEvent = false;
		negMin = 20; negMax = 40;
		posMin = 20; posMax = 40;
		
		numLines = 30;
		numHlines = 5;
		
		for(int i = 0; i < numLines; i++){
		
			lineRots.push_back(360 * i/numLines);
			origRots.push_back(360 * i/numLines);
		
		}
		
		for(int i = 0; i < numHlines; i++){
		
			hLines.push_back(ofVec2f(0,0));
		}
		
		
	};
	
	int numLines, numHlines;
	vector<float> origRots;
	vector<float> lineRots;
	vector<ofVec2f>hLines;
	

	void update(float t_size, ofVec2f t_pos){
		
		if(newEvent){
			
			newEvent = true;

			
			
		}else if(eventTime == 0){
			
			for(int i= 0; i < numLines; i ++){
			
				lineRots[i] += ofRandom(-5,5);
				lineRots[i] = max(origRots[i] - 360/numLines, lineRots[i]); 
				lineRots[i] = min(origRots[i] + 360/numLines, lineRots[i]); 
			}
			
		}
		
		if(eventTime > 0){
					
			for(int i =0; i < hLines.size(); i++){
				hLines[i].y = ofRandom(-1,1);
				hLines[i].x = sqrt(1 - pow(hLines[i].y,2));			
			}
		}
		
		size = t_size;
		pos = t_pos;
		numpoints = min(3000,(int)size * 10);
		
	}
	
	void draw(){
		
		boost::mt19937 g(1);
		boost::normal_distribution<> norm(0,0.5);
		boost::variate_generator<boost::mt19937&,boost::normal_distribution<> > rng(g, norm);
		rng.engine().seed(ofGetFrameNum());
		norm.reset();
		
		
		glPushMatrix();
		glTranslatef(pos.x,pos.y ,0);
		
		ofSetColor(0);
		ofFill();
		ofCircle(0, 0, size* 0.2);
		
		ofSetColor(255);
		

		

		
		glBegin(GL_POINTS);
		for(int i = 0; i < numpoints; i ++){
			float r = ofRandom(0,1);
			
			if(r < 0.9){
				ofVec2f p(ofRandom(-1,1) * size * 0.025,(0.2 + abs(rng() * 0.8)) * size);
				p.rotate(lineRots[i%numLines],ofVec2f(0,0));
				glVertex2f(p.x,p.y);
			}else if(eventTime > 0){
				
				ofVec2f p(ofRandom(-0.2,0.2) * size * hLines[i%numHlines].x,ofRandom(-0.05,0.05) * size * 0.2 + (hLines[i%numHlines].y * 0.2 * size));
				glVertex2f(p.x,p.y);
			}
			
		}
		glEnd();
		

		
		glPopMatrix();
		
	};
	
	
	
};


#endif
