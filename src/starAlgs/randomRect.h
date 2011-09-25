/*
 *  randomRect.h
 *  dsGraphics
 *
 *  Created by Simon Katan on 25/09/2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "activeStarBase.h";

class randomRect : public activeStarBase{
	
	public:
	
	randomRect(){
		
		rot = 0;
		rot_add = 0;
		col = 0;
		count = 0;
		speed = 3;
		size = 1;
		
	};
	
	ofRectangle rect;
	float rot, rot_add;
	ofColor col;
	int count, speed;
	int m_count;
	
	
	void update(float size, ofVec2f pos, bool isMove){
		
		
		if(isMove != this->isMove){
		
			m_count = ofRandom(40,80);
			speed = (isMove) ?  ofRandom(4,7) : ofRandom(1,2);
			
		}else if(m_count > 0){m_count -= 1;
		}else{speed = 2;}
		
		this->size = size;
		this->pos = pos;
		this->isMove = isMove;
		
		if(count == 0){
			rot_add = ofRandom(-20,20);
			rect.set(ofRandom(-20,20), ofRandom(-20,20), ofRandom(max_size/4,max_size), ofRandom(max_size/4,max_size));
			rot += 0.1;
			col.set(ofRandom(200,255), ofRandom(200,255), ofRandom(200,255), 255);
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
