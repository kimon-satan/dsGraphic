#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetCircleResolution(50);
	receiver.setup( PORT );
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	outputMode = 1;
	int rows = 30;
	int columns = 40;
	numStars = rows * columns;
	showPoints = false;
	
	screenWidth = ofGetScreenWidth();
	screenHeight = ofGetScreenHeight();
	radius = screenHeight;
	circum = screenWidth;
	
	bg.loadImage("hubble.jpg");
	ttf.loadFont("verdana.ttf", 70, true, true);
	
	int noise = 6;
	int count = 0;
	for(int i = 0; i < columns; i ++){ 
		for(int j = 0; j < rows; j ++){ 
			star newStar;
			newStar.worldCircum = circum;
			newStar.pos = ofVec2f(-circum/2 + (i+1) * circum/columns, -radius/2 + (j+1) * radius/rows);
			ofVec2f displace(ofRandom(-noise,noise), ofRandom(-noise,noise));
			newStar.pos += displace;
			newStar.id = count;
			newStar.activeStarList = &activeStarList;
			stars.push_back(newStar);
			count ++;
		} 
	}
	
	for(int i =0; i < 20; i++)dsUsers[i].isActive = false;
	
	testIndex = 0;
	distThresh = 1000;
	testPoint = false;
	
	
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
	
	if(outputMode == 2){
		
		for(int i = 0; i < numStars; i ++){
			
			stars[i].update();
			
			if(!stars[i].twinkling){

				float twink = ofRandom(0,1);
				if(twink <= (float)1/(numStars * 10)){
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
				
				for(int j = 0; j < numStars; j++){
					
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
		
		/*string point_stats = "";
		
		for(int i = 0; i < activeList.size(); i++){
			point_stats += "userId: " + ofToString(activeList[i], 0);
			point_stats += "   point deviation: " + ofToString(dsUsers[i].getSDev(),3);
			point_stats += "\n";
			
		}
		
		ofSetColor(100, 100, 100);
		ofDrawBitmapString(point_stats, 50, 50);*/
		
	}else if(outputMode == 2){
		
		ofBackground(0);
		//bg.draw(0,0,circum/2,radius);
		ofSetColor(0,0,50);
		ofFill();
		
		//ofCircle(screenWidth/2,screenHeight/2, screenHeight/2);
		
		ofSetColor(255,255,255);
		glPushMatrix();
		glTranslatef(screenWidth/2,screenHeight/2,0);
		for(int i = 0; i < stars.size(); i ++){stars[i].draw();}
		
		
		if(showPoints){
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
		}
		
		glPopMatrix();
		
	}else if(outputMode == 3){
	
		
		
	}
	
	
	
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
