#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "dsUser.h"
#include "star.h"
#include "ofxBlurShader.h"

#define PORT 12345
#define	HOST "localhost"
#define SCPORT 57120

struct whomp{

	float length;
	int count;

};

class testApp : public ofBaseApp{
public:
	
	void setup();
	void update();
	void draw();
	

	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);		
	void exit();

private:
	
	int findColumn(float x);
	void pairPointsnStars();
	void manageStars();
	void manageWhomps(int sleepCount = -1);
	
	ofxOscReceiver	receiver;
	ofxOscSender	sender;
	ofTrueTypeFont ttf;
	ofImage bg;
	
	float screenWidth, screenHeight, columnWidth;
	ofxBlurShader blurBG;
	ofxBlurShader blurFG;
	float bg_alpha;
	float intensity;
	
	vector <int> activeList;
	dsUser dsUsers[20];
	
	int outputMode;
	int calibStage, calibCount;
	
	vector< vector<star> > stars2d;
	vector<star *> activeStarList;
	vector <whomp> whomps;
	
	int numStars;
	double distThresh;
	bool showPoints;
	
	bool testPoint;
	int testIndex;
	float radius, circum;
	
};
