/*
 *  greyRect.h
 *  dsGraphics
 *
 *  Created by Simon Katan on 26/09/2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

/*
 *  greyRect.h
 *  dsGraphics
 *
 *  Created by Simon Katan on 25/09/2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef GREY_RECT
#define GREY_RECT

#include "activeStarBase.h";

class greyRect : public activeStarBase{
	
public:
	
	greyRect(){
		
		rot = 0;
		rot_add = 0;
		col = 0;
		count = 0;
		speed = 3;
		size = 1;
		starName = "greyRect";
		newEvent = false;
		col.set(80,80,80,255);
		
		negMin = 80; negMax = 160;
		posMin = 6; posMax = 10;
		
	};
	
	ofRectangle rect;
	float rot, rot_add;
	ofColor col;
	int count, speed;
	int m_count;
	
	
	void update(float size, ofVec2f pos){
		
		if(newEvent){
			
			newEvent = true;
			if(eventPolarity > 0){  
				col.set(255,255,255,255);
			}
			
		}else if(eventTime == 0){
			col.set(80,80,80,255);
		}else if(eventTime > 0  && eventPolarity < 0){
			(ofRandom(0,1) > 0.3) ? col.set(0,255):col.set(80,255);
			
		}
		
		this->size = size;
		this->pos = pos;
		
		if(count == 0){
			rot_add = ofRandom(-20,20);
			rect.set(ofRandom(-20,20), ofRandom(-20,20), ofRandom(max_size/4,max_size), ofRandom(max_size/4,max_size));
			rot += 0.1;
		}
		
		count = (count + 1)%speed;
		
	}
	
	void draw(bool isAlpha){
		
		glPushMatrix();
		glTranslatef(pos.x,pos.y ,0);
		glScalef(size/max_size,size/max_size, 1);
		
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
		
	};
	
	
};
#endif