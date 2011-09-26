/*
 *  activeStarBase.h
 *  dsGraphics
 *
 *  Created by Simon Katan on 25/09/2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef ACTIVE_STAR_BASE
#define ACTIVE_STAR_BASE

class activeStarBase{
	
public:
	
	ofVec2f pos;
	float size, max_size;
	string starName;
	int eventTime, eventPolarity;
	int negMin, negMax;
	int posMin, posMax;
	bool newEvent;
	
	
	virtual void update(float size, ofVec2f pos){
	
	}
	
	virtual void draw(bool isAlpha){
	
	}
	
	virtual void reset(){
		
	}
	
	
};

#endif
