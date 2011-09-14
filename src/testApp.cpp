#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	receiver.setup( PORT );
	ofSetFrameRate(60);
	outputMode = 1;
	numStars = 200;
	
	screenWidth = ofGetScreenWidth();
	screenHeight = ofGetScreenHeight();
	
	for(int i = 0; i < 20; i++)dsUsers[i].setUp();
	
	ttf.loadFont("verdana.ttf", 70, true, true);
	
	for(int i = 0; i < numStars; i ++){
	
		star newStar;
		newStar.pos.set(min(ofRandom(0,screenWidth/2),ofRandom(0,screenWidth/2)), 
						min(ofRandom(0,screenWidth/2),ofRandom(0,screenWidth/2)));
		
		if(ofRandom(0,1) > 0.49)newStar.pos.x *= -1;
		if(ofRandom(0,1) > 0.49)newStar.pos.y *= -1;
		
		//bias towards centre
		
		newStar.size = ofRandom(0.5,3);
		newStar.id = i;
		stars.push_back(newStar);
		
		
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
		
		if	(m.getAddress() == "/calib" )
		{
			calibStage = m.getArgAsInt32(0);
			calibCount = m.getArgAsInt32(1); 
		}
	}
	
	
	for(int i =0; i< activeList.size(); i++){
		
		dsUsers[activeList[i]].updateHistory();
		
		if(dsUsers[activeList[i]].ghostCount > 10){
			dsUsers[activeList[i]].active = false;
			activeList.erase(activeList.begin() + i);
			
		}
		
	}
	
	for(int i = 0; i < numStars; i ++){stars[i].update();}
	
}


//--------------------------------------------------------------
void testApp::draw(){
	
	if(outputMode == 1){
		
		ofBackground( 255, 255, 255 );
		ofSetColor(150);
		float col = screenWidth/12;
		float row = screenHeight/8;
		ofNoFill();
		for(int i = 1; i < 11; i++)ofLine(0, row * i, screenWidth, row*i);
		for(int i = 1; i < 12; i++)ofLine(col * i, 0, col * i, screenHeight);
		ofCircle(screenWidth/2, screenHeight/2, 25);
		
		
		ofSetColor(255, 0, 0);
		switch (calibStage) {
			case 1:
				ttf.drawString(ofToString(calibCount,0), 20, 70);
				break;
				
			case 2:
				ttf.drawString(ofToString(calibCount,0), screenWidth - 70, screenHeight - 30);
				break;
				
			case 3:
				ttf.drawString(ofToString(calibCount,0), 20, screenHeight - 30);
				break;
				
			case 10:
				ttf.drawString("FAILED", screenWidth/2 - 150, screenHeight/2 - 25);
				break;
				
		}
		
		glPushMatrix();
		glTranslatef(screenWidth/2,screenHeight/2,0);
		
		ofFill();
		for(int i =0; i <activeList.size(); i++){
			ofCircle(dsUsers[activeList[i]].avPos.x, 
					 dsUsers[activeList[i]].avPos.y, 
					 10);
			
		}
		
		
		glPopMatrix();
		
	}else if(outputMode == 2){
		
		ofBackground(0);
		
		ofSetColor(255,255,255);
		glPushMatrix();
		glTranslatef(screenWidth/2,screenHeight/2,0);
		for(int i = 0; i < stars.size(); i ++){
		
			ofCircle(stars[i].pos.x, stars[i].pos.y, stars[i].size);
			
		}
		
		
		ofFill();
		for(int i =0; i <activeList.size(); i++){
			ofCircle(dsUsers[activeList[i]].avPos.x, 
					 dsUsers[activeList[i]].avPos.y, 
					 10);
			
		}
		
		glPopMatrix();
		
	}
	
	
	
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
