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
	
	dsUser(){
		
		reset();
		isActive = true;
	
	}
	
	void reset(){
		
		pos.set(0,0);
		isActive = true;
		historySize = 3;
		isPaired = false;	
		ghostCount = 0;
		isMoving = false;
		history.clear();
		stillCount = 0;
	}
	
	void updateHistory(){
		
		history.push_back(pos);
		if(history.size() > historySize){
			history.erase(history.begin());
		}
		
		avPos = ofVec2f(0,0);
		for(int j = 0; j < historySize; j ++){
			avPos += history[j];
		}
		
		avPos /= history.size(); 
		if(ghostCount > 0) ghostCount += 1;
		
		if(!isMoving){ 
			stillCount += 1;
		}else{ stillCount = 0;}
	}
	
	float getSDev(){
		
		float sd;
		
		for(int i = 0; i < history.size(); i++)sd += avPos.distance(history[i]);

		sd /= history.size();
		
		return sd; //I know this isn't sd but I don't have internet !
	}
	
	
	int historySize;
	int ghostCount;
	int stillCount;
	ofVec2f avPos;
	ofVec2f pos;
	vector<ofVec2f>  history;
	bool isActive, isPaired, isMoving ,isStill, isFake;
	int starId;
	
};


#endif