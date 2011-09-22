#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetCircleResolution(50);
	receiver.setup( PORT );
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	outputMode = 1;
	int rows, cols;
	showPoints = false;
	
	screenWidth = ofGetScreenWidth();
	screenHeight = ofGetScreenHeight();
	radius = screenHeight;
	circum = screenWidth;
	
	blurBG.setup(screenWidth, screenHeight);
	blurFG.setup(screenWidth, screenHeight);
	
	float h, s, offset;
	rows = 10;
	s = screenHeight/(rows * 2.8);
	h =  s * sqrt(3.0f);
	cols = screenWidth/h + 1;
	offset = h/4.0f;
	
	int noise = 3;
	int count = 0;
	
	
	for(int i = 0; i < cols; i ++){
		
		for(int j = 0; j < rows; j ++){	
			ofVec2f positions[4] = {
				ofVec2f(i * h, j * s * 3),
				ofVec2f(i * h, j * s * 3 + s),
				ofVec2f(-h/2 + h * i, -offset + j * s * 3),
				ofVec2f(-h/2 + h * i, -offset + j * s * 3 + s * 2)
			};
			
			for(int k =0; k < 4; k ++){
				star newStar;
				newStar.worldCircum = circum;
				newStar.pos = ofVec2f(positions[k].x - screenWidth/2, positions[k].y - screenHeight/2);
				ofVec2f displace(ofRandom(-noise,noise), ofRandom(-noise,noise));
				newStar.pos += displace;
				newStar.id = count;
				newStar.activeStarList = &activeStarList;
				stars.push_back(newStar);
				count ++;
			}
		} 
	}
	

	for(int i =0; i < 20; i++)dsUsers[i].isActive = false;
	
	testIndex = 0;
	distThresh = 1000;
	testPoint = false;
	bg.loadImage("moonTest.jpg");
	
	glDisable(GL_DEPTH_TEST); 
	
	
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
			dsUsers[id].reset();
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
			
			dsUsers[id].isFake = m.getArgAsInt32(3);
			
		}
		
		if	(m.getAddress() == "/calib" )
		{
			calibStage = m.getArgAsInt32(0);
			calibCount = m.getArgAsInt32(1); 
		}
		
		if (m.getAddress() == "/isMoving"){
		
			int id = m.getArgAsInt32(0);
			dsUsers[id].isMoving = (bool)m.getArgAsInt32(1);

		}
	}
	
	
	for(int i =0; i< activeList.size(); i++){
		
		dsUsers[activeList[i]].updateHistory();
		
		if(dsUsers[activeList[i]].ghostCount > 10){
			dsUsers[activeList[i]].isActive = false;
			dsUsers[activeList[i]].isPaired = false;
			activeList.erase(activeList.begin() + i);
		}
		
	}
	
	if(outputMode >= 2){
		
		for(int i = 0; i < stars.size(); i ++){
			
			stars[i].update();
			
			if(!stars[i].twinkling){

				float twink = ofRandom(0,1);
				if(twink <= (float)1/(stars.size())){
				stars[i].twinkle();
					
				} //should equal one every 2 secs
			}
		
		}
		
		manageStars();
		pairPointsnStars();
		
		
	}
	
}


void testApp::pairPointsnStars(){
	
	for(int i = 0; i < activeList.size(); i++){
		
		if(!dsUsers[activeList[i]].isPaired){
			
			if(dsUsers[activeList[i]].stillCount > 30){
				
				float dist = pow(distThresh,2);
				int starId = -1;
				
				for(int j = 0; j < stars.size(); j++){
					
					if(!stars[j].isActive && !stars[j].isCovered){ 
						float td = stars[j].pos.squareDistance(dsUsers[activeList[i]].pos); //brute force search for nearest star
						if(td < dist){
							starId = j; 
							dist = td;
						}										//maybe optimise with a look up table.
					}
				}
				
				if(starId > 0){ 
					
					dsUsers[activeList[i]].isPaired = true;
					dsUsers[activeList[i]].starId = starId;
					stars[starId].isActive = true;
					stars[starId].pairedUser = &dsUsers[activeList[i]];
					activeStarList.push_back(&stars[starId]);
				}
			}
		}
		
	}
}

void testApp::manageStars(){
	
	//first tidy up the active stars
		
	for(int i = 0; i < activeStarList.size(); i ++){
		
		if(!activeStarList[i]->isActive)activeStarList.erase(activeStarList.begin() + i);
		
	} 
	

}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	
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
			
			
			if(dsUsers[activeList[i]].isMoving){
				ofCircle(dsUsers[activeList[i]].pos.x, 
						 dsUsers[activeList[i]].pos.y, 
						 10);
				}else{
					
					if(dsUsers[activeList[i]].isFake){
						ofNoFill();
					}else{
						ofFill();
					}
					   ofSetRectMode(OF_RECTMODE_CENTER);
					   ofRect(dsUsers[activeList[i]].pos.x, 
							  dsUsers[activeList[i]].pos.y,
							  20,20);
					   ofSetRectMode(OF_RECTMODE_CORNER);
				}
			
		}
		
		glPopMatrix();

		
	}else if(outputMode == 2){
		
		ofBackground(0);
		
		
		
		glPushMatrix();
		glTranslatef(screenWidth/2,screenHeight/2,0);
		for(int i = 0; i < stars.size(); i ++)stars[i].drawBG(false); 
		for(int i = 0; i < stars.size(); i ++)stars[i].drawActiveAlgorithm(false); 
		glPopMatrix();
		
		if(showPoints){
			
			glPushMatrix();
			glTranslatef(screenWidth/2,screenHeight/2,0);
			ofFill();
			for(int i =0; i <activeList.size(); i++){
				if(dsUsers[activeList[i]].isMoving){
					ofCircle(dsUsers[activeList[i]].pos.x, 
							 dsUsers[activeList[i]].pos.y, 
							 10);
				}else{
					ofSetRectMode(OF_RECTMODE_CENTER);
					ofRect(dsUsers[activeList[i]].pos.x, 
						   dsUsers[activeList[i]].pos.y,
						   10,10);
					ofSetRectMode(OF_RECTMODE_CORNER);
				}
			}
			glPopMatrix();
		}
			 
		
	}else if(outputMode == 3){
		
		
		
		blurBG.begin(1,2);
		ofClear(0, 0, 0, 255); //black bg
		glPushMatrix();
		glTranslatef(screenWidth/2, screenHeight/2, 0);
		for(int i = 0; i < stars.size(); i ++)stars[i].drawBG(false);
		glPopMatrix();
		blurBG.end();
		
		blurFG.begin(1,1);
		glPushMatrix();
		glTranslatef(screenWidth/2, screenHeight/2, 0);
		for(int i = 0; i < stars.size(); i ++)stars[i].drawActiveAlgorithm(false); 
		glPopMatrix();
		blurFG.end();
		
		ofSetColor(255);
		blurBG.draw(); 
		
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		float a = ofMap(mouseX, 0, screenWidth, 0, 100, false);
		ofSetColor(230,255,230, a);
		bg.draw(0,0,screenWidth,screenHeight);
		ofDisableBlendMode();
		
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		glPushMatrix();
		glTranslatef(screenWidth/2, screenHeight/2, 0);
		for(int i = 0; i < stars.size(); i ++)stars[i].drawBG(true);
		glPopMatrix();
		ofSetColor(255, 255, 255, 255);
		ofDisableBlendMode();
		
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		
		ofSetColor(255);
		blurFG.draw(); 
		
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		glPushMatrix();
		glTranslatef(screenWidth/2, screenHeight/2, 0);
		for(int i = 0; i < stars.size(); i ++)stars[i].drawActiveAlgorithm(true); 
		glPopMatrix();
		ofSetColor(255, 255, 255, 255);
		ofDisableBlendMode();
		
	}
	
	ofSetColor(100, 100, 100);
	ofDrawBitmapString(ofToString(ofGetFrameRate(),2), 20,20);
	
}




//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	
	if(key == 'f')ofToggleFullscreen();
	if(key == 'm')outputMode += 1; outputMode = outputMode%4;
	if(key == 's')showPoints = !showPoints;
	if(key == 't')testPoint = true;
	if(key == '+')testIndex = (testIndex +1)%3;
	if(key == 'd'){stars[100].twinkle();}
	
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
	
	if(testPoint){
		
		if(!dsUsers[testIndex].isActive){
			dsUsers[testIndex].reset();
			activeList.push_back(testIndex);
			dsUsers[testIndex].pos.set(x -screenWidth/2,y-screenHeight/2);	
		}else{
			for(int i =0; i< activeList.size(); i++){
				if(activeList[i] == testIndex){
					dsUsers[testIndex].isActive = false;
					dsUsers[testIndex].isPaired = false;
					activeList.erase(activeList.begin() + i);
				}
			}
		}
	}
	
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

void testApp::exit(){

cout << "exit \n";
	
}
