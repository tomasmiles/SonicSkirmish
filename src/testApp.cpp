#include "testApp.h"

testApp::gameState state;
testApp::aiState ai;

maxiOsc oscBank[24];
maxiEnvelope adsr;
double cooldownEnv[24];
int liveOscs = 24;
double adsrArgs[8] = {1, 50, 0.75, 10, 1, 50, 0, 100};

//--------------------------------------------------------------
void testApp::setup(){
       notes.resize(24);
        notes[0] = 130.81; //c3
        notes[1] = 138.59; //c#3
        notes[2] = 146.83; //d3
        notes[3] = 155.56; //d#3
        notes[4] = 164.81; //e3
        notes[5] = 174.61; //f3
        notes[6] = 185.00; //f#3
        notes[7] = 196.00; //g3
        notes[8] = 207.65; //g#3
        notes[9] = 220.00; //a3
        notes[10] = 233.08; //a#3
        notes[11] = 246.94; //b3
        notes[12] = 130.81; //c4
        notes[13] = 138.59; //c#4
        notes[14] = 146.83; //d4
        notes[15] = 155.56; //d#4
        notes[16] = 164.81; //e4
        notes[17] = 174.61; //f4
        notes[18] = 185.00; //f#4
        notes[19] = 196.00; //g4
        notes[20] = 207.65; //g#4
        notes[21] = 220.00; //a4
        notes[22] = 233.08; //a#4
        notes[23] = 246.94; //b4

    arena = ofRectangle(float(ofGetWidth())/7, float(ofGetHeight())/7,
                        float(ofGetWidth())/7*6 - float(ofGetWidth())/7,
                        float(ofGetHeight())/7*6 - float(ofGetHeight())/7);

    ofSetFrameRate(50);
    ofSetVerticalSync(true);

    playing = false;
    state = STATE_MAIN_MENU;


    aiClock = 0;
    ai = ASSESS;
    testerNumber = 24;
    testers.resize(testerNumber);
    float blueTeamY = (arena.getHeight()/7) + arena.getTop();
    float redTeamY = (arena.getHeight()/7*6) + arena.getTop();
    float unitSpacing = arena.getWidth()/float((testerNumber/2)+1);
    for(int a = 0; a < testerNumber; a++)
    {
        if(a < testers.size()/2)
        {
            testers[a] = unit(((a+1)*unitSpacing)+arena.getLeft(),blueTeamY,0);
            testers[a].setTeam(0);
        }else
        {
            testers[a] = unit(((a+1 - testers.size()/2)*unitSpacing)+arena.getLeft(),redTeamY,180);
            testers[a].setTeam(1);
        }
    }
    entities = entitySystem(testers);
    entities.setArena(arena.getLeft(), arena.getTop(), arena.getWidth(), arena.getHeight());

    shiftKeyDown = false;
    rightMouseDown = false;
    controlKeyDown = false;

    mainMenu.setup();
    mainMenu.add(title.setup("Sonic Skirmish","An Audio-Visual Real-Time Strategy"));
    mainMenu.add(preGameButton.setup("New Game"));
    mainMenu.add(optionsButton.setup("Options"));

    optionsMenu.setup();
    optionsMenu.add(optionsMainMenuButton.setup("Quit to Main Menu"));
    optionsMenu.add(muteAIButton.setup("Mute AI"));
    optionsMenu.add(unmuteAIButton.setup("Unmute AI"));

    pregameMenu.setup();
    pregameMenu.add(newGameButton.setup("Start game"));
    pregameMenu.add(mainMenuButton.setup("Quit to Main Menu"));

    pregameMenu.add(unitEditor.setup());
    unitEditor.add(unitNumber.setup("Unit",0, 0, 11, 256, 15));
    unitEditor.add(noteNumber.setup("Note",0, 0, 23, 256, 15));
    unitEditor.add(strength.setup("Missile Strength", 1, 1, 10, 200, 15));

    gameMenu.setup();
    gameMenu.add(pauseButton.setup("Pause [p]"));
    gameMenu.add(mainMenuButton.setup("Quit to Main Menu"));

    unitNumber.addListener(this, &testApp::unitChanged);
    noteNumber.addListener(this, &testApp::noteChanged);
    strength.addListener(this, &testApp::strengthChanged);

    preGameButton.addListener(this, &testApp::preGame);
    newGameButton.addListener(this, &testApp::newGame);
    optionsButton.addListener(this, &testApp::options);
    optionsMainMenuButton.addListener(this, &testApp::exitToMainMenu);
    mainMenuButton.addListener(this, &testApp::exitToMainMenu);
    pauseButton.addListener(this, &testApp::pause);
    muteAIButton.addListener(this, &testApp::mute);
    unmuteAIButton.addListener(this, &testApp::unmute);
    //pauseButton.addListener(this, &testApp::pause);
    maxiSettings::setup(44100,1,512);
    ofSoundStreamSetup(maxiSettings::channels,0,this,maxiSettings::sampleRate, maxiSettings::bufferSize, 4);
}

void testApp::preGame()
{
    playing = false;
    state = STATE_PREGAME;
    gameOver = false;

    aiClock = 0;
    ai = ASSESS;
    testerNumber = 24;
    testers.resize(testerNumber);
    float blueTeamY = (arena.getHeight()/7) + arena.getTop();
    float redTeamY = (arena.getHeight()/7*6) + arena.getTop();
    float unitSpacing = arena.getWidth()/float((testerNumber/2)+1);
    for(int a = 0; a < testerNumber; a++)
    {
        if(a < testers.size()/2)
        {
            testers[a] = unit(((a+1)*unitSpacing)+arena.getLeft(),blueTeamY,0);
            testers[a].setTeam(0);
            testers[a].frequency = notes[6];
        }else
        {
            testers[a] = unit(((a+1 - testers.size()/2)*unitSpacing)+arena.getLeft(),redTeamY,180);
            testers[a].setTeam(1);
            testers[a].frequency = notes[18];
        }

    }
    entities = entitySystem(testers);
    entities.setArena(arena.getLeft(), arena.getTop(), arena.getWidth(), arena.getHeight());
}

void testApp::newGame(){
    playing = true;
    state = STATE_GAME;
    gameOver = false;
    for(int i = 0; i < 24; i++) cooldownEnv[i] = 0;
}

void testApp::continueGame(){
    playing = false;
    state = STATE_GAME;
}

void testApp::pause(){
    if(playing){
            playing = false;
            for(int i = 0; i < 24; i++) cooldownEnv[i] = 0;
    }
    else if(!playing) playing = true;
    }

void testApp::options(){
    state = STATE_OPTIONS;
    }

void testApp::exitToMainMenu(){
    if(state == STATE_GAME && gameOver == false)
    {
         mainMenu.add(continueGameButton.setup("Continue Game"));
         continueGameButton.addListener(this, &testApp::continueGame);
         for(int i = 0; i < 24; i++) cooldownEnv[i] = 0;
    }
    state = STATE_MAIN_MENU;
}

void testApp::unitChanged(int &unitNumber){
noteNumber = entities.units[unitNumber].note;
strength = entities.units[unitNumber].missileStrength;
}

void testApp::noteChanged(int &noteNumber){
    entities.units[unitNumber].note = noteNumber;
    entities.units[unitNumber].frequency = notes[noteNumber];
    entities.units[unitNumber+12].note = noteNumber;
    entities.units[unitNumber+12].frequency = notes[noteNumber];
}

void testApp::strengthChanged(int &strength){
    entities.units[unitNumber].missileStrength = strength;
    entities.units[unitNumber].cooldownMax = strength * 50;
    entities.units[unitNumber+12].missileStrength = strength;
    entities.units[unitNumber+12].cooldownMax = strength * 50;
    }

void testApp::mute(){
    liveOscs = 12;
    for(int i = 0; i < 24; i++) cooldownEnv[i] = 0;
}

void testApp::unmute(){
    liveOscs = 24;
}

void testApp::exit(){
}

void testApp::isGameOver()
{
if(!entities.blueTeamAlive() || !entities.redTeamAlive())gameOver = true;
}

//--------------------------------------------------------------
void testApp::update(){
    mousePos = ofVec2f(mouseX, mouseY);
    if(state == STATE_GAME && playing)
        {
            //AI control
            int cpuUnitsLeft = 0;
            for(int x = 0; x < entities.units.size(); x++)if(entities.units[x].team == entities.computerTeam && entities.units[x].alive)cpuUnitsLeft++;
            if(aiClock%(1 + cpuUnitsLeft)==0)
            {
                switch(ai)
                {
                    case ASSESS:
                        {
                            unitPositions.erase(unitPositions.begin(),unitPositions.end());
                            enemyPositions.erase(enemyPositions.begin(),enemyPositions.end());
                            for(int i = 0; i < entities.units.size(); i++)
                            {
                                if(entities.units[i].alive)
                                {
                                    if(entities.units[i].team == 0)enemyPositions.push_back(entities.units[i].position);
                                    else if(entities.units[i].team == 1)unitPositions.push_back(entities.units[i].position);
                                }
                            }
                            float leftMost = unitPositions[0].x;
                            float rightMost = unitPositions[0].x;
                            float topMost = unitPositions[0].y;
                            float bottomMost = unitPositions[0].y;
                            for(int j = 1; j < unitPositions.size(); j++)
                            {
                                if(unitPositions[j].x < leftMost)leftMost = unitPositions[j].x;
                                else if(unitPositions[j].x > rightMost)rightMost = unitPositions[j].x;
                                if(unitPositions[j].y < topMost)topMost = unitPositions[j].y;
                                else if(unitPositions[j].y > bottomMost)bottomMost = unitPositions[j].y;
                            }
                            float radius = entities.units[0].radius;
                            topLeft = ofVec2f(leftMost-radius, topMost-radius);
                            bottomRight = ofVec2f(rightMost+radius, bottomMost+radius);
                            ai = SELECT_START;
                            }
                    break;

                    case SELECT_START:
                        aiSelectStart(topLeft.x, topLeft.y);
                        ai = SELECT_END;
                    break;

                    case SELECT_END:
                        aiSelectEnd(bottomRight.x, bottomRight.y);
                        ai = BOX_SELECT;
                    break;

                    case BOX_SELECT:
                        aiBoxSelect();
                        ai = ORDER_MOVE;
                    break;

                    case ORDER_MOVE:
                        {
                            ofVec2f destination = ofVec2f(ofRandom(arena.getLeft(),arena.getRight()),ofRandom(arena.getTop(),arena.getBottom()));
                            aiOrderMove(destination.x,destination.y,destination.x+entities.units[entities.cpuSelectedUnitIndex[0]].position.x,
                                        destination.y+entities.units[entities.cpuSelectedUnitIndex[0]].position.y);
                            float moving = ofRandom(0,100);
                            if(moving < 50)ai = ORDER_ATTACK;
                            if(moving >= 50)ai = ORDER_MOVE;
                        }
                    break;

                    case ORDER_ATTACK:
                        int target;
                        for(int x = 0; x < entities.units.size();x ++)
                        {
                            if(!entities.units[x].team == entities.computerTeam && entities.units[x].alive)
                            {
                                target = x;
                                //break;
                            }
                        }
                        for(int i = 1; i <entities.units.size(); i++)
                        {
                            if(!entities.units[i].team == entities.computerTeam && entities.units[i].alive)
                            {
                                if(entities.units[i].healthPoints < entities.units[target].healthPoints)target = i;
                            }
                        }
                        aiOrderAttack(target);
                        ai = ASSESS;
                        break;

                    case WAIT:
                        break;
                    }
                }

            entities.update();
            for(int x = 0; x < 24; x++)
                {
                    if(entities.units[x].alive)
                    {
                        cooldownEnv[x] = double(entities.units[x].cooldown) / double(entities.units[x].cooldownMax);
                    }else cooldownEnv[x] = 0;
                }
            aiClock++;
            isGameOver();

        }
}

void testApp::aiSelectStart(float x, float y)
{
entities.cpuBoxSelectActive = true;
entities.cpuBoxStart(x, y);
entities.cpuBoxEnd(x, y);
}

void testApp::aiSelectEnd(float x, float y)
{
    entities.cpuBoxEnd(x, y);
}

void testApp::aiBoxSelect()
{
    entities.cpuBoxSelect();
    entities.cpuBoxSelectActive = false;
}

void testApp::aiOrderMove(float x, float y, float dirX, float dirY)
{
entities.orderUnit(x, y, dirX, dirY, 1);
}

void testApp::aiOrderAttack(int i)
{
    entities.orderUnit(entities.units[i].position.x, entities.units[i].position.y,
                       entities.units[i].position.x, entities.units[i].position.y, 1);
}
//--------------------------------------------------------------
void testApp::draw(){
    ofBackgroundHex(0x000000);
    if(state == STATE_MAIN_MENU){
            //Main menu loop
            mainMenu.draw();
    }else
    if(state == STATE_OPTIONS){
            //Options loop
            optionsMenu.draw();

    }else
    if(state == STATE_PREGAME){
       //Pre-game loop
        pregameMenu.draw();
        for(int i = 0; i < testerNumber/2; i++){
        //The player's team
        ofDrawBitmapString("Unit " + ofToString(i), 20 + i*100, 200);
        ofFill();
        ofSetColor(0,255,0);
        ofRect(20 + i*100, 205, entities.units[i].healthPoints*2, 15);
        ofSetColor(255,255,255);
        ofDrawBitmapString("HP: " + ofToString(entities.units[i].healthPoints), 20 + i*100, 220);
        ofSetColor(0,0, 255);
        ofRect(20 + i*100, 225, entities.units[i].note*4, 15);
        ofSetColor(255,255,255);
        ofDrawBitmapString("Note: " + ofToString(entities.units[i].note), 20 + i*100, 240);
        ofSetColor(255 ,0, 0);
        ofRect(20 + i*100, 245, entities.units[i].missileStrength*10, 15);
        ofSetColor(255,255,255);
        ofDrawBitmapString("Damage: " + ofToString(entities.units[i].missileStrength), 20 + i*100, 260);
        ofSetColor(255, 0, 0);
        ofRect(20 + i*100, 265, entities.units[i].cooldownMax/5, 15);
        ofSetColor(255,255,255);
        ofDrawBitmapString("Reload: " + ofToString(entities.units[i].cooldownMax), 20 + i*100, 280);
        //The CPU team
        ofDrawBitmapString("Unit " + ofToString(i+12), 20 + i*100, 400);
        ofSetColor(0,255,0);
        ofRect(20 + i*100, 405, entities.units[i+12].healthPoints*2, 15);
        ofSetColor(255,255,255);
        ofDrawBitmapString("HP: " + ofToString(entities.units[i+12].healthPoints), 20 + i*100, 420);
        ofSetColor(0,0, 255);
        ofRect(20 + i*100, 425, entities.units[i+12].note*4, 15);
        ofSetColor(255,255,255);
        ofDrawBitmapString("Note: " + ofToString(entities.units[i+12].note), 20 + i*100, 440);
        ofSetColor(255 ,0, 0);
        ofRect(20 + i*100, 445, entities.units[i+12].missileStrength*10, 15);
        ofSetColor(255,255,255);
        ofDrawBitmapString("Damage: " + ofToString(entities.units[i+12].missileStrength), 20 + i*100, 460);
        ofSetColor(255, 0, 0);
        ofRect(20 + i*100, 465, entities.units[i+12].cooldownMax/5, 15);
        ofSetColor(255,255,255);
        ofDrawBitmapString("Reload: " + ofToString(entities.units[i+12].cooldownMax), 20 + i*100, 480);

        ofNoFill();
        ofSetColor(255, 255, 255);
        ofRect(20, 180, 1200, 400);
        ofLine(20, 380, 1220, 380);
        for(int z = 1; z < 13; z++)
            {
                ofLine(20 + z*100, 180, 20 + z*100, 580);
            }
        }
        ofSetColor(0,255,0);
        ofRect(20 + unitNumber*100, 180, 100, 200);
        ofRect(20 + unitNumber*100, 380, 100, 200);
    }else
    if(state == STATE_GAME){
            //Game loop
            gameMenu.draw();
            entities.draw();
            ofNoFill();
            if(rightMouseDown)drawFormationDirection();
            ofSetColor(255,255,255);
            ofRect(arena);
            if(gameOver)ofDrawBitmapString("GAME OVER", ofGetWidth()/2, ofGetHeight()/2);
            ofDrawBitmapString("Signal:" + ofToString(signal), 250, 25);
    }
    ofSetColor(255,255,255);
    ofDrawBitmapString(ofToString(ofGetFrameRate())+ " FPS", 20, 25);

}

void testApp::audioRequested(float *output, int bufferSize, int nChannels){
    for(int i = 0; i < bufferSize; i++)
    {
        double oscSum = 0;
        int missileTotalAll = 0;

        for(int j = 0; j < liveOscs; j++)
        {
            oscSum += (oscBank[j].square(notes[entities.units[j].note])*cooldownEnv[j]);
        }
        oscSum /= float(liveOscs);
        oscSum *= 0.99;

        output[i] = oscSum;
        signal = oscSum;
    }
}

void testApp::drawFormationDirection()
{
    ofSetColor(0,255,0);
    ofLine(rightMouseHeldPos.x, rightMouseHeldPos.y, mousePos.x, mousePos.y);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(state == STATE_GAME)
    {
        if(key == OF_KEY_SHIFT)
        {
            shiftKeyDown = true;
        }else
        if(key == OF_KEY_CONTROL)
        {
            //cout << "control key pressed \n";
            controlKeyDown = true;
        }
        if(controlKeyDown)
        {
            entities.assignControlGroup(key);
        }else entities.selectControlGroup(key);
        if(key == 'p'||key == 'P')
        {
            pause();
        }
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    if(state == STATE_GAME)
    {
        if(key == OF_KEY_SHIFT)
        {
            shiftKeyDown = false;
        }
        if(key == OF_KEY_CONTROL)
        {
            controlKeyDown = false;
        }
    }
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    if(state == STATE_GAME)
    {
        entities.boxEnd(x, y);
    }
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    if(state == STATE_GAME)
    {
        if(button == 0)
        {
            entities.boxSelectActive = true;
            entities.boxStart(x, y);
            entities.boxEnd(x,y);
        }
        if(button == 2)
        {
            rightMouseDown = true;
            rightMouseHeldPos= mousePos;
        }
    }
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    if(state == STATE_GAME)
    {
        if(button == 0)
        {
            entities.boxSelectActive = false;
            entities.boxSelect();
            entities.pointSelect(mousePos.x, mousePos.y, entities.humanTeam);

            //entities.cpuBoxSelectActive = false;
            //entities.boxSelect(entities.computerTeam);
            //entities.pointSelect(mousePos.x, mousePos.y, entities.computerTeam);
        }
        if(button == 2)
        {
            rightMouseDown = false;
            if(!shiftKeyDown)
            {
                entities.orderUnit(rightMouseHeldPos.x, rightMouseHeldPos.y, mousePos.x,mousePos.y, 0);
            }else {entities.orderUnitQueue(rightMouseHeldPos.x, rightMouseHeldPos.y, mousePos.x,mousePos.y, 0);}
        }
    }
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
