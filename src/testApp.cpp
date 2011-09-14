#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	receiver.setup( PORT );
	ofSetFrameRate(60);
	
	screenWidth = ofGetScreenWidth();
	screenHeight = ofGetScreenHeight();
	
	for(int i = 0; i < 20; i++){
		
		dsUsers[i].setUp();
	}
	
}

//--------------------------------------------------------------
void testApp::update(){
	
	// check for waiting messages
	while( receiver.hasWaitingMessages() )
	{
		
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage( &m );
		
		if ( m.getAddress() == "/init" )
		{
			cout << "init \n";
			activeList.clear();
		}
		
		
		if ( m.getAddress() == "/outputMode" )
		{
			outputMode = m.getArgAsInt32(0);
		}
		
		if ( m.getAddress() == "/newUser" )
		{
			int id = m.getArgAsInt32(0);
			dsUsers[id].pos = ofVec2f(0,0);
			dsUsers[id].ghostCount = 0;
			activeList.push_back(id);
		}
		
		if	(m.getAddress() == "/lostUser" )
		{
			int id = m.getArgAsInt32(0);
			dsUsers[id].ghostCount = 1;
		}
		
		// check for point message
		if ( m.getAddress() == "/point" )
		{
			int id = m.getArgAsInt32(0);
			dsUsers[id].ghostCount = 0;
			dsUsers[id].pos.set(
								m.getArgAsFloat(1) * screenWidth,
								-m.getArgAsFloat(2) * screenHeight
								);
			
		}
	}
	
	
	for(int i =0; i< activeList.size(); i++){
		
		dsUsers[activeList[i]].updateHistory();
		
		if(dsUsers[activeList[i]].ghostCount > 10){
			dsUsers[activeList[i]].active = false;
			activeList.erase(activeList.begin() + i);
			
		}
		
	}
	
	
	
}


//--------------------------------------------------------------
void testApp::draw(){
	
	ofBackground( 255, 255, 255 );
	ofSetColor(150);
	
	glPushMatrix();
	glTranslatef(screenWidth/2,screenHeight/2,0);
	
	for(int i =0; i <activeList.size(); i++){
		
		ofFill();
		ofCircle(dsUsers[activeList[i]].avPos.x, 
				 dsUsers[activeList[i]].avPos.y, 
				 10);
		
	}
	
	ofNoFill();
	ofCircle(0, 0, 50);
	glPopMatrix();
	
	
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	
	if(key == 'f')ofToggleFullscreen();
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
	
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 
	
}
