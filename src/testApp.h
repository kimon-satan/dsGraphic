#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "dsUser.h"
#include "star.h"

#define PORT 12345

class testApp : public ofBaseApp{
public:
	
	void setup();
	void update();
	void draw();
	
	void pairPointsnStars();
	void manageStars();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);		

private:
	ofxOscReceiver	receiver;
	ofTrueTypeFont ttf;
	
	float screenWidth, screenHeight;
	
	vector <int> activeList;
	dsUser dsUsers[20];
	
	int outputMode;
	int calibStage, calibCount;
	
	vector<star> stars;
	vector<star *> activeStarList;
	int numStars;
	double distThresh;
	bool showPoints;
	
};
