/*
 *  star.h
 *  DS_Graphics
 *
 *  Created by Simon Katan on 14/09/2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef STAR
#define STAR

#include "ofMain.h" 
#include "dsUser.h"

struct rectAlgo{
	
	rectAlgo(){
		
		rot = 0;
		rot_add = 0;
		col = 0;
	};
	
	ofRectangle rect;
	float rot;
	float rot_add;
	ofColor col;

	void draw(ofVec2f pos, bool isAlpha, float active_size){
		
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
	
	};

	
};

class star{

public:

	star();
	~star();
	
	void update();
	void findConflicts();
	void findIsCovered();
	void drawBG(bool isAlpha);
	void drawActiveAlgorithm(bool isAlpha);
	void twinkle();
	
	float rotSpeed;
	ofVec2f rot_axis;
	ofVec2f pos;
	float size;
	float active_size;
	float intensity;
	float alpha;
	float worldCircum;
	int id;
	bool twinkling;
	
	dsUser * pairedUser;
	bool isActive;
	bool isCovered;
	float base;
	float add;
	float inc;
	int count;
	float mul;

	ofColor tc;
	vector<star*>* activeStarList;
	rectAlgo rA;
	
};


#endif