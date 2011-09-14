/*
 *  dsUser.h
 *  DS_Graphics
 *
 *  Created by Simon Katan on 14/09/2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef DS_USER
#define DS_USER

class dsUser{
	
public:
	
	dsUser(){};
	
	void setUp(){
		pos.set(0,0);
		active = false;
		historySize = 40;
		
		for(int i = 0; i < historySize; i ++){
			history.push_back(pos);	
		}
		
		ghostCount = 20;
		
	};
	

	
	void updateHistory(){
		
		static int i = 0;
		i = (i+1)%historySize;
		history[i] = pos;

		if(!active){
			for(int j = 0; j < historySize; j ++){
				history[j] = pos;
			}
			active = true;
		}
		
		avPos = ofVec2f(0,0);
		for(int j = 0; j < historySize; j ++){
			avPos += history[j];
		}
		
		avPos /= historySize; 
		if(ghostCount > 0) ghostCount += 1;
	}
	
	
	int historySize;
	int ghostCount;
	ofVec2f avPos;
	ofVec2f pos;
	vector<ofVec2f>  history;
	bool active;
	
};


#endif