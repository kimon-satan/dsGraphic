/*
 *  rotShrinkFrames.h
 *  dsGraphics
 *
 *  Created by Simon Katan on 26/09/2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef ROT_SHRINK_FRAMES
#define ROT_SHRINK_FRAMES

#include "activeStarBase.h";

class rotShrinkFrames : public activeStarBase{
	
public:
	
	vector<float> squareSizes;
	vector<float> squareRots;
	vector<ofVec2f> addVecs;
	float mul;
	int thisEvent;
	
	rotShrinkFrames(){
		
		starName = "rotShrinkFrames";
		newEvent = false;
		negMin = 10; negMax = 15;
		posMin = 80; posMax = 160;
	
		reset();
		mul = 0;
	};
	
	void reset(){
	
		squareRots.clear();
		squareSizes.clear();
		addVecs.clear();
			
		squareSizes.push_back(max_size);
		squareRots.push_back(1);
		addVecs.push_back(ofVec2f(ofRandom(-1,1),ofRandom(-1,1)).normalize());
	}
	
	
	void update(float size, ofVec2f pos){
		
		if(newEvent){
			
			newEvent = true;
			thisEvent = eventTime;
			mul = 0;
			
		}else if(eventPolarity > 0 && eventTime > 0){
			 
			mul = max_size * 0.5 * sin(ofDegToRad((float)eventTime * 180.0f/(float)thisEvent));
			for(int i = 0; i < squareSizes.size(); i ++){
				squareSizes[i] = 400 * sin(ofDegToRad((ofGetFrameNum() + i * 40)/2));
				squareRots[i] += (float)1/(i+1);
			}
			
		}else if(eventPolarity < 0 && eventTime > 0){
			
			for(int i = 0; i < squareRots.size(); i++)squareRots[i] = ofRandom(0,10);  //glitch
																						//maybe something selse is better ?
			
		}else if(eventTime == 0){
			
			mul = 0;
			float scale = size/max_size;
			
			int numsquares;
			
			if(scale < 0.2){
			numsquares = (int)min(max(1.0f,scale * 100 - 1),10.0f);
			}else{
			numsquares = (int)min(max(1.0f,scale * 40 - 1),25.0f);
			}
			
			
			if(numsquares > squareSizes.size()){
				squareRots.push_back(ofRandom(0,90));
				squareSizes.push_back(max_size);
				ofVec2f av(ofRandom(-1,1),ofRandom(-1,1));
				addVecs.push_back(av.normalize());
				
			}else if(numsquares < squareSizes.size()){
				squareRots.pop_back();
				squareSizes.pop_back();
				addVecs.pop_back();
			}
			
			for(int i = 0; i < squareSizes.size(); i ++){
				squareSizes[i] = 400 * sin(ofDegToRad((ofGetFrameNum() + i * 40)/2));
				squareRots[i] += (float)1/(i+1);
			}
		}
		
		
		this->size = size;
		this->pos = pos;
		
	};
	
	void draw(bool isAlpha){
		
		if(isAlpha){ //noBlurring on this alg
			
			float scale = size/max_size;
			
			glPushMatrix();
			glTranslatef(pos.x,pos.y ,0);
			glScalef(size/max_size,size/max_size, 1);
			
			ofNoFill();
			ofSetColor(255, 255);
			ofSetRectMode(OF_RECTMODE_CENTER);
			
			
			for(int i = 0; i < squareSizes.size(); i ++){
				glPushMatrix();
				glRotatef(squareRots[i], 0, 0, 1);
				ofRect(mul * addVecs[i].x,mul * addVecs[i].y , squareSizes[i], squareSizes[i]);
				glPopMatrix();
			}
			
			ofSetRectMode(OF_RECTMODE_CORNER);
			glPopMatrix();
			
		}
		
	};
	
	
	
	
};

#endif
