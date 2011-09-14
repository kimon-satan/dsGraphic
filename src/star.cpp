/*
 *  star.cpp
 *  DS_Graphics
 *
 *  Created by Simon Katan on 14/09/2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "star.h"


star::star(){

	rotSpeed = min(ofRandom(0.1,0.5), ofRandom(0.1,0.5));
	rot_axis.set(ofRandom(-20,20), ofRandom(-20,20));

}

void star::update(){
	
	pos.rotate(rotSpeed,rot_axis);
	
}

void star::draw(){
	
}


star::~star(){


}