/*
 *  statFlow.h
 *  dsGraphics
 *
 *  Created by Simon Katan on 27/09/2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef STAT_FLOW
#define STAT_FLOW

#include "activeStarBase.h";

class statFlow : public activeStarBase{
	
public:
	
	statFlow(){
		
		starName = "statFlow";
		newEvent = false;
		negMin = 500; negMax = 500;
		posMin = 10; posMax = 10;
		sd_edge = 0.25;
		sd_center = 0.25;
		count = 270;
	};
	
	int numpoints, eventLength, count;
	float sd_center, sd_edge, e_phase, e_mul, c_mul, c_phase, o_width,  i_width, o_mul, i_mul;
	
	void update(float t_size, ofVec2f t_pos){
		
		if(newEvent){
		
			if(ofRandom(0,1) > 0.15){
			
				newEvent = false; //override to reduce number of events
				eventTime = 0;
			}
		}
		
		if(newEvent){
			
			if(eventPolarity < 0){
				
				eventLength = eventTime;
				e_mul = 60.0f/eventLength;
				c_mul = 0.36 * 5/eventLength;
				sd_center = 0.001;
				o_width = 0.7;
				i_width = 0.7;
				o_mul = 0.6 * 2/eventLength;
				i_mul = 0.69 * 2/eventLength;
			}
			
		}else if(eventTime > 0 && eventPolarity < 0){
			
			int ePassed = eventLength - eventTime;
			c_phase = 1;
			count = 270;
			e_phase = abs(sin(ofDegToRad(210 - (float)ePassed * e_mul)));
			if((float)ePassed/eventLength > 0.5){
				o_width -= o_mul;
				i_width -= i_mul;
				if((float)ePassed/eventLength > 0.8)sd_center += c_mul;
			}
			
		}else if(eventTime == 0 || eventPolarity > 0){
		
			c_phase = abs(sin(ofDegToRad(count/3)));
			sd_center = 0.1 + c_phase * 0.25;
			count = (count + 1);
			
		}
	
		
		size = t_size;
		pos = t_pos;
		
		numpoints = min(3000,(int)size * 10);
		
	}
	
	void draw(){
		
		
		boost::mt19937 g_cent(1);
		boost::normal_distribution<> norm_cent(0,sd_center);
		boost::variate_generator<boost::mt19937&,boost::normal_distribution<> > rng_cent(g_cent, norm_cent);
		
		boost::mt19937 g_edge(1);
		boost::normal_distribution<> norm_edge(0,sd_edge);
		boost::variate_generator<boost::mt19937&,boost::normal_distribution<> > rng_edge(g_edge, norm_edge);
		
		rng_edge.engine().seed(ofGetFrameNum());
		norm_edge.reset();
		rng_cent.engine().seed(ofGetFrameNum());
		norm_cent.reset();
		
		
		ofSetColor(255);
		glPushMatrix();
		glTranslatef(pos.x,pos.y ,0);
		
		glBegin(GL_POINTS);
		for(int i = 0; i < numpoints; i++)
		{
			
			float e_dist, c_dist;
			
			c_dist =  rng_cent();
			e_dist =  rng_edge();
			
			if(eventTime > 0 && eventPolarity < 0){
				
				e_dist = (e_dist > 0) ? e_dist * (1-e_phase) * o_width + 0.9: e_dist * e_phase * i_width + 0.9;
				
			}else{
				e_dist = (e_dist > 0) ? e_dist * 0.1 + 0.9 : e_dist * 0.01 + 0.9; 
			}
			
			
			ofVec2f	p_e = (0,e_dist * size);
			ofVec2f p_c = (0,c_dist * size);
			
			float r = ofRandom(0,360);
			
			p_e.rotate(r);
			glVertex2f(p_e.x, p_e.y);
			
			if((float)eventTime/eventLength < 0.4 ||eventTime == 0 || eventPolarity > 0){
			p_c.rotate(r);
			glVertex2f(p_c.x, p_c.y);
			}			
		}
		glEnd();
		glPopMatrix();

		
	};
	
	
};

#endif
