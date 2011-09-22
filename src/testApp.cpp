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
	rows = 11;
	s = screenHeight/(rows * 2.8);
	h =  s * sqrt(3.0f);
	cols = screenWidth/h + 1;
	offset = h/4.0f;
	
	int noise = 4;
	int count = 0;
	
	
	for(int i = 0; i < cols; i ++){
		
		vector<star> s1;
		vector<star> s0;
		
		for(int j = 0; j < rows; j ++){	
			ofVec2f positions[4] = {
				ofVec2f(i * h, j * s * 3),
				ofVec2f(i * h, j * s * 3 + s),
				ofVec2f(h/2 + h * i, -offset + j * s * 3),
				ofVec2f(h/2 + h * i, -offset + j * s * 3 + s * 2)
			};
			
			for(int k =0; k < 4; k ++){
				star newStar;
				newStar.worldCircum = circum;
				newStar.rotSpeed = -screenWidth/(pow(60.0f,2) * 2);
				newStar.pos = ofVec2f(positions[k].x - screenWidth/2, positions[k].y - screenHeight/2);
				ofVec2f displace(ofRandom(-noise,noise), ofRandom(-noise,noise));
				newStar.pos += displace;
				newStar.id = count;
				newStar.activeStarList = &activeStarList;
				(k < 2) ? s0.push_back(newStar): s1.push_back(newStar);
				count ++;
			}
		}
		
		stars2d.push_back(s0);
		stars2d.push_back(s1);
	}
	
	numStars = count;

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
		
		for(int i = 0; i < stars2d.size(); i ++){
			for(int j = 0; j < stars2d[i].size(); j ++){
			
			stars2d[i][j].update();
			
			/*if(!stars2d[i][j].twinkling){

				float twink = ofRandom(0,1);
				if(twink <= (float)1/numStars)stars2d[i][j].twinkle();
					
				} //should equal one every 2 secs */
			}
		
		}
		
		static int msecs = 0;
		static int currentCol = findColumn(-screenWidth/2);
		static int timeInterval = (30000/stars2d.size()) * 0.75;
		static int colCount = 0;
		static int sleepTime = 30;
			
			if(sleepTime < 30 & ofGetElapsedTimeMillis() > msecs){
				
				sleepTime += 1;
				msecs = ofGetElapsedTimeMillis() + 1000;
		
			}else if(sleepTime == 30 && ofGetElapsedTimeMillis() > msecs){	
				if(colCount == 0){
					currentCol = findColumn(-screenWidth/2);
				}
				msecs = ofGetElapsedTimeMillis() + timeInterval;
				currentCol = (currentCol + 1)%stars2d.size();
				colCount += 1;
				if(colCount == stars2d.size()){
					colCount = 0;
					sleepTime = 0;
					msecs = ofGetElapsedTimeMillis() + 1000;
				}
				
			}else if(colCount > 0){
				
				int numCols = min(5, colCount);
				
				for(int i = 0; i < 1; i++){
					int c = currentCol - i;
					if(c < 0)c = c + stars2d.size(); //wrapping
						for(int j = 0; j < stars2d[c].size(); j ++){
					
							if(!stars2d[c][j].twinkling){
							 
							  float twink = ofRandom(0,1);
								if(twink <= (float)1/(5 * stars2d[c].size())) stars2d[c][j].twinkle();
								
							 } 
						}
					
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
				star * st_pnt = NULL;
				
				int col = findColumn(dsUsers[activeList[i]].pos.x );
				
				for(int j =0; j < stars2d[col].size(); j++){
					if(!stars2d[col][j].isActive && !stars2d[col][j].isCovered){ 
						float td = stars2d[col][j].pos.squareDistance(dsUsers[activeList[i]].pos); //brute force search for nearest star
						if(td < dist){
							st_pnt = &stars2d[col][j]; 
							dist = td;
						}										//maybe optimise with a look up table.
					}
				}
				
				
				if(st_pnt != NULL){ 
					
					dsUsers[activeList[i]].isPaired = true;
					dsUsers[activeList[i]].starId = st_pnt->id;
					st_pnt->isActive = true;
					st_pnt->pairedUser = &dsUsers[activeList[i]];
					activeStarList.push_back(st_pnt);
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

int testApp::findColumn(float x){
	
	static float colwidth = screenWidth/stars2d.size();
	float offsetX = x - stars2d[0][0].pos.x;
	if(offsetX < 0){offsetX += screenWidth;}
	int col  = offsetX/colwidth; 
	return col;
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
		for(int i = 0; i < stars2d.size(); i ++){ 
			for(int j = 0; j < stars2d[i].size(); j ++){ 
			stars2d[i][j].drawBG(false);
			}
		}
		
		for(int i = 0; i < stars2d.size(); i ++){ 
			for(int j = 0; j < stars2d[i].size(); j ++){ 
				stars2d[i][j].drawActiveAlgorithm(false); 
			}
		}
		
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
		for(int i = 0; i < stars2d.size(); i ++){ 
			for(int j = 0; j < stars2d[i].size(); j ++){ 
			stars2d[i][j].drawBG(false);
			}
		}
		
		glPopMatrix();
		blurBG.end();
		
		blurFG.begin(1,1);
		glPushMatrix();
		
		glTranslatef(screenWidth/2, screenHeight/2, 0);
		for(int i = 0; i < stars2d.size(); i ++){ 
			for(int j = 0; j < stars2d[i].size(); j ++){ 
				stars2d[i][j].drawActiveAlgorithm(false); 
			}
		}
		
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
		
		for(int i = 0; i < stars2d.size(); i ++){ 
			for(int j = 0; j < stars2d[i].size(); j ++){ 
				stars2d[i][j].drawBG(true);
			}
		}
		
		glPopMatrix();
		ofSetColor(255, 255, 255, 255);
		ofDisableBlendMode();
		
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		
		ofSetColor(255);
		blurFG.draw(); 
		
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		glPushMatrix();
		glTranslatef(screenWidth/2, screenHeight/2, 0);
		for(int i = 0; i < stars2d.size(); i ++){ 
			for(int j = 0; j < stars2d[i].size(); j ++){ 
				stars2d[i][j].drawActiveAlgorithm(true); 
			}
		}
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
	if(key == 'd'){stars2d[10][5].twinkle();}
	
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
