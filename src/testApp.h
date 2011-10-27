#pragma once

#include <algorithm>
#include "ofMain.h"
#include "ofxOsc.h"
#include "dsUser.h"
#include "star.h"
#include "ofxBlurShader.h"
#include "ofxXmlSettings.h"

#define PORT 12345
#define	HOST "localhost"
#define SCPORT 57120

using namespace std;

class testApp : public ofBaseApp{
public:
	
	void setup();
	void update();
	void draw();
	void drawRaw();
	void drawBlurred();
	void drawTest();
	void saveMappingPoints();
	void loadMappingPoints();
	


	
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
	
	ofxOscReceiver	receiver;
	ofxOscSender	sender;
	ofTrueTypeFont ttf;
	ofImage bg, sp;
	
	float screenWidth, screenHeight, columnWidth;
	ofxBlurShader blurBG;
	ofxBlurShader blurFG;
	float bg_alpha, intensity, rotSpeed;
	
	
	vector <dsUser *> activeList;
	dsUser dsUsers[20];
	
	int outputMode;
	int calibStage, calibCount;
	
	vector< vector<star *> > stars2d;
	vector<star *> activeStarList;
	
	int numStars;
	double distThresh;
	bool showPoints;
	
	bool testPoint;
	int testIndex;
	float radius, circum;
	ofFbo fbo_tex;
	ofTexture map_tex;

	GLUquadricObj *quadric;
	float w_prop, h_prop;
	float sphereRot;
	
	vector<ofPoint> t_points;
	vector<ofPoint> v_points;
	vector<ofPoint> o_points;
	
	int gridSize, selPoint;
	
};
