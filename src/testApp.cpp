#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	//ofSetCircleResolution(30);
	
	ofDisableArbTex(); 
 
	
	receiver.setup( PORT );
	sender.setup(HOST, SCPORT);
	
	ofxOscMessage m;
	m.setAddress("/init");
	sender.sendMessage(m);
	
	//ofSetVerticalSync(true);
	ofSetFrameRate(60);
	outputMode = 1;
	int rows, cols;
	showPoints = false;
	
	screenHeight = ofGetScreenHeight();
	screenWidth = ofGetScreenHeight();
	
	radius = screenHeight;
	circum = screenWidth * 1.25;
	
	blurBG.setup(screenWidth, screenHeight);
	blurFG.setup(screenWidth, screenHeight);
	
	float h, s, offset;
	rows = 18;
	s = screenHeight/(rows * 2.8);
	h =  s * sqrt(3.0f);
	cols = circum/h;
	offset = h/4.0f;
	bg_alpha = 45;
	selPoint = 0;
	
	int noise = 2;
	int count = 0;
	rotSpeed = -screenWidth/3600;
	

	
	for(int i = 0; i < cols; i ++){
		
		vector<star *> s1;
		vector<star *> s0;
		
		for(int j = 0; j < rows; j ++){	
			ofVec2f positions[4] = {
				ofVec2f(i * h, j * s * 3),
				ofVec2f(i * h, j * s * 3 + s),
				ofVec2f(h/2 + h * i, -offset + j * s * 3),
				ofVec2f(h/2 + h * i, -offset + j * s * 3 + s * 2)
			};
			
			for(int k =0; k < 4; k ++){
				star * newStar = new star();
				newStar->worldCircum = circum;
				newStar->worldHeight = screenHeight;
				newStar->rotSpeed = rotSpeed; //rotation once every 15 secs
				newStar->pos = ofVec2f(positions[k].x - circum/2, positions[k].y - screenHeight/2);
				ofVec2f displace(ofRandom(-noise,noise), ofRandom(-noise,noise));
				newStar->pos += displace;
				newStar->id = count;
				newStar->assignAlgorithm(i%5); ///(i%5);
				newStar->setupAttributes();
				
								
				newStar->activeStarList = &activeStarList;
				if(k < 2){  
					newStar->col = i * 2;
					s0.push_back(newStar);
				}else{ 
					newStar->col = i * 2 + 1;
					s1.push_back(newStar);
				}
				
				
				
				count ++;
			}
		}
		
		stars2d.push_back(s0);
		stars2d.push_back(s1);
	}
	
	numStars = count;
	columnWidth = circum/stars2d.size();

	for(int i =0; i < 20; i++){
		dsUsers[i].id = i;
		dsUsers[i].isActive = false;
	}
	
	testIndex = 0;
	distThresh = 200;
	testPoint = false;
	bg.loadImage("moonTest.jpg");
	sp.loadImage("sphere.jpg");
	
	fbo_tex.allocate(screenWidth, screenHeight, GL_RGBA); 
	map_tex.allocate(screenWidth, screenHeight, GL_RGBA);

	w_prop = (float)screenWidth/ofNextPow2(screenWidth);
	h_prop = (float)screenHeight/ofNextPow2(screenHeight);
	
	gridSize = 11;
	
	for(int y= 0; y < gridSize; y++){
		for(int x= 0; x < gridSize; x++){
			
			ofPoint p((float)x/(gridSize-1),(float)y/(gridSize-1));
			ofPoint np = p * ofPoint(screenWidth,screenHeight);
			v_points.push_back(np);
			o_points.push_back(p);
			p *= ofPoint(w_prop,h_prop);
			t_points.push_back(p);
			
			
		}
	}
	
	sphereRot = 0;
	loadMappingPoints();
	
	
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
			ofxOscMessage m;
			m.setAddress("/init");
			sender.sendMessage(m);
		}
		
		
		if ( m.getAddress() == "/outputMode" )
		{
			outputMode = m.getArgAsInt32(0);
		}
		
		if ( m.getAddress() == "/newUser" )
		{
			int id = m.getArgAsInt32(0);
			if(!dsUsers[id].isActive){
				dsUsers[id].reset();
				activeList.push_back(id);
			}else{
				dsUsers[id].ghostCount = 0;
			}
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
								m.getArgAsFloat(1) * -screenWidth,
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
			cout << i << ":" << "end user " << ofGetFrameNum() << "\n";
			break;
		}
		
	}
	
	
	
	if(outputMode >= 2){
		//sphereRot -= 0.05;
		sphereRot = ofMap(mouseX, 0, ofGetScreenWidth(), 0, 360, false);
		pairPointsnStars();
		manageStars();
		
	}
	
}



void testApp::manageStars(){
	
	
	vector <int> eraseList;
	
	//first tidy up the active stars
		
	for(int i = 0; i < activeStarList.size(); i ++){
		
		if(!activeStarList[i]->isActive){
			int col = findColumn(activeStarList[i]->pos.x);
			
			if(activeStarList[i]->col != col){
				stars2d[col].push_back(activeStarList[i]); // put a star pointer into the relevant column - ONLY if a different column
				activeStarList[i]->col = col;
			}
			
			activeStarList[i]->twinkle(5);
			ofxOscMessage m;
			m.setAddress("/endStar");
			m.addIntArg(activeStarList[i]->pairedUser->id);
			sender.sendMessage(m);
			
			
				cout << i << ":" << "end active " << ofGetFrameNum() <<"\n";
				activeStarList.erase(activeStarList.begin() + i);
			break;
			
			
			
		}		
	} 
	
	
	//then call update on all stars
	for(int i = 0; i < stars2d.size(); i ++){
		for(int j = 0; j < stars2d[i].size(); j ++){
			if(stars2d[i][j]->col != i ){
				stars2d[i].erase(stars2d[i].begin() + j); //delete old references
				j -= 1;
				
			}else{
				stars2d[i][j]->update(); 
				
				if(stars2d[i][j]->isActive){
				
					float a = stars2d[i][j]->active_size/stars2d[i][j]->max_size; //there could ultimately be different max_sizes
					ofxOscMessage m;
					m.setAddress("/updateStar");
					m.addIntArg(stars2d[i][j]->pairedUser->id); //the users id for no searching in sclang
					m.addFloatArg(a); // normalised value for size;
					m.addIntArg(stars2d[i][j]->activeStar->newEvent);
					m.addIntArg(stars2d[i][j]->activeStar->eventTime);
					m.addIntArg(stars2d[i][j]->activeStar->eventPolarity);
					m.addFloatArg(stars2d[i][j]->pos.x/screenWidth);
					sender.sendMessage(m);
				}
			}
		}
	}
	
	
	//now handle star lighting
	
	static int msecs = 0;
	static int currentCol = findColumn(-screenWidth/2);
	static int colCount = 0;
	static int sleepTime = 27;
	static int finalMills = 0;
	
	if(sleepTime < 30){
		for(int i = 0; i < stars2d.size(); i ++){  //random twickling ...maybe iterative is not necessary 
			for(int j = 0; j < stars2d[i].size(); j ++){
				
				if(!stars2d[i][j]->twinkling){
					
					float twink = ofRandom(0,1);
					if(twink <= (float)1/numStars * 1/20){
						
							if(stars2d[i][j]->size < 3 && !stars2d[i][j]->isActive)stars2d[i][j]->twinkle(ofRandom(10,20));
					}
					
				} 
				
			} 
		}
	}

	//time counting wave  of stars
	
	int waveWidth = 7;
	int distance = screenWidth + (-rotSpeed * 1800) + (waveWidth-1) * columnWidth;
	int timeInterval = 30000 * columnWidth/distance;
	


	if(sleepTime < 30 & ofGetElapsedTimeMillis() > msecs){
		
		sleepTime += 1;
		msecs = ofGetElapsedTimeMillis() + 1000;
		
		if(sleepTime == 30){
			ofxOscMessage m; //tell superCollider to begin wave
			m.setAddress("/wave");
			sender.sendMessage(m);
		}
		
	}else if(sleepTime == 30 && ofGetElapsedTimeMillis() > msecs){	
		
		if(colCount == 0){ //start of wave
			currentCol = findColumn(-screenWidth/2); //find the first column
			finalMills = (30000 - (waveWidth * 220)) + ofGetElapsedTimeMillis();
		}
		
		msecs = ofGetElapsedTimeMillis() + timeInterval;
		currentCol = (currentCol + 1)%stars2d.size();
		colCount += 1;
		
		if(ofGetElapsedTimeMillis() > finalMills){
			colCount = 0;
			sleepTime = 0;
			msecs = ofGetElapsedTimeMillis() + 1000;
		}
		
	}else if(colCount > 0){
		
		int numCols = min(waveWidth, colCount);
		
		for(int i = 0; i < numCols; i++){
			int c = currentCol - i;
			if(c < 0)c = c + stars2d.size(); //wrapping
			for(int j = 0; j < stars2d[c].size(); j ++){
				
				if(!stars2d[c][j]->twinkling){
					
					float twink = ofRandom(0,1);
					if(twink <= (float)1/(2 * stars2d[c].size())){
						stars2d[c][j]->twinkle(ofRandom(100,220));
						
					}
					
				} 
			}
			
		}
		
		
	}

	

}

int testApp::findColumn(float x){
	
	float offsetX = x - stars2d[0][0]->pos.x;
	if(offsetX < 0){offsetX += circum;}
	int col  = offsetX/columnWidth; 
	return col;
}



void testApp::pairPointsnStars(){
	
	for(int i = 0; i < activeList.size(); i++){
		
		if(!dsUsers[activeList[i]].isPaired){
			
			if(dsUsers[activeList[i]].stillCount > 20){
				
				float dist = pow(distThresh,2);
				star * st_pnt = NULL;
				
				
				
				//there was a crash here
				for(int k = 0; k < stars2d.size(); k++){
					for(int j =0; j < stars2d[k].size(); j++){
						if(!stars2d[k][j]->isActive && !stars2d[k][j]->isCovered && stars2d[k][j]->intensity > 0){ 
							float td = stars2d[k][j]->pos.squareDistance(dsUsers[activeList[i]].pos); //brute force search for nearest star
							if(td < dist){
								st_pnt = stars2d[k][j]; 
								dist = td;
							}										//maybe optimise with a look up table.
						}
					}
				}
				
				
				if(st_pnt != NULL){ 
					
					cout << i << ":" << "pair " << ofGetFrameNum();
					dsUsers[activeList[i]].isPaired = true;
					dsUsers[activeList[i]].starId = st_pnt->id;
					st_pnt->isActive = true;
					st_pnt->pairedUser = &dsUsers[activeList[i]];
					activeStarList.push_back(st_pnt);
					
					ofxOscMessage m;
					m.setAddress("/newStar");
					m.addIntArg(st_pnt->pairedUser->id); //unique ref to star 
					m.addStringArg(st_pnt->activeStar->starName);
					sender.sendMessage(m);
					
					
				}
			}
		}
		
	}
}

void testApp::saveMappingPoints(){
	
	ofxXmlSettings XML;
	int tagNum = XML.addTag("MAPPINGPOINTS");
	if( XML.pushTag("MAPPINGPOINTS", tagNum) ){
		
		for(int i = 0; i < v_points.size(); i++){
			
			o_points[i] = v_points[i]/ofVec2f(screenWidth,screenHeight);
			
			tagNum = XML.addTag("POINT");
			if(XML.pushTag("POINT", tagNum)){
				XML.setValue("X", o_points[i].x);
				XML.setValue("Y", o_points[i].y);
				XML.popTag();
			}
			
		}
		
		XML.popTag();
	}
	
	XML.saveFile("mappingPoints.xml");

}

void testApp::loadMappingPoints(){

	ofxXmlSettings XML;
	XML.loadFile("mappingPoints.xml");

	if( XML.pushTag("MAPPINGPOINTS", 0) ){
		
		for(int i = 0; i < v_points.size(); i++){
			
			if(XML.pushTag("POINT", i)){
				o_points[i].x = XML.getValue("X",0.0f);
				o_points[i].y = XML.getValue("Y",0.0f);
				v_points[i] = o_points[i] * ofVec2f(screenWidth,screenHeight);
				XML.popTag();
			}
			
		}
		
		XML.popTag();
	}
	
	
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	if(outputMode == 0){
		ofBackground(0);
	}else if(outputMode == 1){
		
		glPushMatrix();
		glTranslatef(ofGetScreenWidth()/2 - screenWidth/2, 0, 0);
		drawTest();
		glPopMatrix();
		
	}else if(outputMode == 2){
		ofBackground(0);
		glPushMatrix();
		glTranslatef(ofGetScreenWidth()/2 - screenWidth/2, 0, 0);
		drawRaw();
		glPopMatrix();
		
	}else if(outputMode == 3){
		ofBackground(0);
		glPushMatrix();
		glTranslatef(ofGetScreenWidth()/2 - screenWidth/2, 0, 0);
		drawBlurred();
		glPopMatrix();
		
	}else if(outputMode == 4 || outputMode == 5){
		
		ofBackground(0);
		fbo_tex.begin();
		ofClear(0, 0, 0, 255);
		glPushMatrix();
		
		if(outputMode == 4){
			drawBlurred();
		}else{
			drawTest();
		}
		
		//drawRaw();
		glPopMatrix();
		fbo_tex.end();
	
		ofPixels pixels;
		fbo_tex.readToPixels(pixels);
		map_tex.loadData(pixels);
		
		ofBackground(0);
		ofSetColor(255,255,255,255);
		//bind and draw texture
		//glEnable(GL_DEPTH_TEST);  
		glPushMatrix();
		glTranslatef(ofGetScreenWidth()/2 - screenWidth/2, 0, 0);
		
		map_tex.bind();
		glBegin(GL_QUADS);
		
		for(int y = 0; y < gridSize - 1; y++){
			for(int x = 0; x < gridSize - 1; x++){
				
				int i = x + (gridSize * y);
				glTexCoord2f(t_points[i].x,t_points[i].y); 
				glVertex2i(v_points[i].x, v_points[i].y); // top left 
				
				i += 1;
				glTexCoord2f(t_points[i].x,t_points[i].y); 
				glVertex2i(v_points[i].x, v_points[i].y); // top right 
				
				i += gridSize;
				glTexCoord2f(t_points[i].x,t_points[i].y); 
				glVertex2i(v_points[i].x, v_points[i].y); // bottom right 
				
				i -= 1;
				glTexCoord2f(t_points[i].x,t_points[i].y); 
				glVertex2i(v_points[i].x, v_points[i].y); // bottom left 
				
				
			}
		}
		
		glEnd();
		
		map_tex.unbind();
		
		glPopMatrix();
		//glDisable(GL_DEPTH_TEST); 
	
	}else if(outputMode == 6){
		
		
		
		ofBackground(255);
		glPushMatrix();
		glTranslatef(ofGetScreenWidth()/2 - screenWidth/2, 0, 0);
		
		//sp.draw(0, 0, screenWidth,screenHeight);
		
		ofBackground(255);
		fbo_tex.begin();
		ofClear(0, 0, 0, 255);
		glPushMatrix();
		
			drawTest();
		
		
		//drawRaw();
		glPopMatrix();
		fbo_tex.end();
		
		ofPixels pixels;
		fbo_tex.readToPixels(pixels);
		map_tex.loadData(pixels);
		
		ofSetColor(255,255,255,255);
		//bind and draw texture
		//glEnable(GL_DEPTH_TEST);  
		glPushMatrix();
		glTranslatef(ofGetScreenWidth()/2 - screenWidth/2, 0, 0);
		
		map_tex.bind();
		glBegin(GL_QUADS);
		
		for(int y = 0; y < gridSize - 1; y++){
			for(int x = 0; x < gridSize - 1; x++){
				
				int i = x + (gridSize * y);
				glTexCoord2f(t_points[i].x,t_points[i].y); 
				glVertex2i(v_points[i].x, v_points[i].y); // top left 
				
				i += 1;
				glTexCoord2f(t_points[i].x,t_points[i].y); 
				glVertex2i(v_points[i].x, v_points[i].y); // top right 
				
				i += gridSize;
				glTexCoord2f(t_points[i].x,t_points[i].y); 
				glVertex2i(v_points[i].x, v_points[i].y); // bottom right 
				
				i -= 1;
				glTexCoord2f(t_points[i].x,t_points[i].y); 
				glVertex2i(v_points[i].x, v_points[i].y); // bottom left 
				
				
			}
		}
		
		glEnd();
		
		map_tex.unbind();
		
		
		//draw image + quad points
		for(int i = 0; i < v_points.size() - 1; i++){
			(i == selPoint ) ? ofSetColor(255, 0, 0):ofSetColor(0,255,0);
			ofCircle(v_points[i].x , v_points[i].y, 2);
			ofSetColor(150);
		}
		glPopMatrix();
	}
	
	ofSetColor(100, 100, 100);
	//ofDrawBitmapString(ofToString(ofGetFrameRate(),2), 20,20);
	
}
void testApp::drawTest(){
	
	ofBackground( 255, 255, 255 );
	ofSetColor(150);
	float col = screenWidth/10;
	float row = screenHeight/10;
	ofNoFill();
	for(int i = 1; i < 12; i++)ofLine(0, row * i, screenWidth, row*i);
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
	

}

void testApp::drawRaw(){
	

	//ofSetColor(0);
	
	
	
	ofNoFill();
	//ofRect(1, 1, screenWidth + 2, screenHeight);
	ofFill();
	

	
	glPushMatrix();
	glTranslatef(screenWidth/2,screenHeight/2,0);
	for(int i = 0; i < stars2d.size(); i ++){ 
		for(int j = 0; j < stars2d[i].size(); j ++){ 
			stars2d[i][j]->drawBG(false);
		}
	}
	
	for(int i = 0; i < stars2d.size(); i ++){ 
		for(int j = 0; j < stars2d[i].size(); j ++){ 
			stars2d[i][j]->drawActiveAlgorithm(); 
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
	
	ofSetColor(230,255,230, 25);
	bg.draw(0, 0,screenWidth * 1.5,screenHeight);

}

void testApp::drawBlurred(){

	blurBG.begin(1,2);
	ofClear(0, 0, 0, 255); //black bg
	glPushMatrix();
	glTranslatef(screenWidth/2, screenHeight/2, 0);
	for(int i = 0; i < stars2d.size(); i ++){ 
		for(int j = 0; j < stars2d[i].size(); j ++){ 
			stars2d[i][j]->drawBG(false);
		}
	}
	
	glPopMatrix();
	blurBG.end();
	
	ofSetColor(255);
	blurBG.draw(); 
	
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofSetColor(230,255,230, bg_alpha);
	bg.draw(0,0,screenWidth,screenHeight);
	ofDisableBlendMode();
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	glPushMatrix();
	glTranslatef(screenWidth/2, screenHeight/2, 0);
	
	for(int i = 0; i < stars2d.size(); i ++){ 
		for(int j = 0; j < stars2d[i].size(); j ++){ 
			stars2d[i][j]->drawBG(true);
		}
	}
	
	glPopMatrix();
	ofSetColor(255, 255, 255, 255);
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofDisableBlendMode();
	
	glPushMatrix();
	glTranslatef(screenWidth/2, screenHeight/2, 0);
	for(int i = 0; i < stars2d.size(); i ++){ 
		for(int j = 0; j < stars2d[i].size(); j ++){ 
			stars2d[i][j]->drawActiveAlgorithm(); 
		}
	}
	glPopMatrix();
	ofSetColor(255, 255, 255, 255);

}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	
	
	if(key == 'f')ofToggleFullscreen();

	if(key == 'm')outputMode += 1; outputMode = outputMode%7;
	if(key == 'b'){
		if(outputMode == 0){outputMode = 4;}else{outputMode = 0;}
		
	}
	

	
	if(outputMode != 6){
		if(key == 's')showPoints = !showPoints;
		if(key == 't')testPoint = true;
		if(key == 'q')dsUsers[activeList[0]].isMoving = !dsUsers[activeList[0]].isMoving;
		if(key == '+')testIndex = (testIndex +1)%3;
	}else{
		switch (key) {
			case '-':
				selPoint = selPoint = max(0, selPoint - 1);
				break;
			case '+':
				selPoint = min((int)o_points.size(), selPoint + 1);
				break;
			case OF_KEY_RIGHT:
				v_points[selPoint].x += 1;
				break;
			case OF_KEY_LEFT:
				v_points[selPoint].x -= 1;
				break;
			case OF_KEY_DOWN:
				v_points[selPoint].y += 1;
				break;
			case OF_KEY_UP:
				v_points[selPoint].y -= 1;
				break;
			case 'S':
				saveMappingPoints();
				break;
				
		}

	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

		for(int i =0; i< activeList.size(); i++){
			dsUsers[activeList[i]].pos.set(x - ofGetScreenWidth()/2, y - screenHeight/2);
		}
	
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
			dsUsers[testIndex].pos.set(x -ofGetScreenWidth()/2,y-screenHeight/2);	
		}else{
			for(int i =0; i< activeList.size(); i++){
				if(activeList[i] == testIndex){
					dsUsers[testIndex].isActive = false;
					dsUsers[testIndex].isPaired = false;
					activeList.erase(activeList.begin() + i);
					break;
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
	
	for(int i = 0; i < stars2d.size(); i ++){
		for(int j = 0; j < stars2d[i].size(); j ++)delete stars2d[i][j]; 
	}
	
	
}
