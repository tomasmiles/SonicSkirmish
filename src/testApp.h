#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxMaxim.h"
#include "entitysystem.h"
#include "missile.h"


class testApp : public ofBaseApp{

	public:
	    //Methods for setting up variables, updating the game and gui state, and for drawing the relevant context
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

        void play();
        void preGame();
        void newGame();
        void continueGame();
        void pause();
        void options();
        void exitToMainMenu();
		void exit();
		void isGameOver();

		void mute();
		void unmute();

		//AI methods
		void aiSelectStart(float x, float y);
		void aiSelectEnd(float x, float y);
		void aiBoxSelect();
		void aiOrderMove(float x, float y, float dirX, float dirY);
		void aiOrderAttack(int i);

        //Main menu
		ofxPanel mainMenu;
		//Options menu
		ofxPanel optionsMenu;
		//Pre-game menu
		ofxPanel pregameMenu;
		//In-game menu
		ofxPanel gameMenu;
		//Title of the game
		ofxLabel title;
		//Go back to the main menu
		ofxButton mainMenuButton;
		ofxButton optionsMainMenuButton;
		//Start a new session
		ofxButton preGameButton;
		ofxButton newGameButton;
		//Resume the current session
		ofxButton continueGameButton;
		//Options
		ofxButton optionsButton;
		//End session
		ofxButton endSessionButton;
		//Pause game
		ofxButton pauseButton;
		//Mute and unmute the ai
		ofxButton muteAIButton;
		ofxButton unmuteAIButton;

        ofxPanel unitEditor;
        ofxIntSlider unitNumber;
		ofxIntSlider noteNumber;
		ofxIntSlider strength;

    //The number of individual attributes should be equal to the number of units * the variety of attribute.
        //vector <> ;


		void unitChanged(int &unitNumber);
		void noteChanged(int &noteNumber);
		void strengthChanged(int &strength);

        //Is the current state a game session?
		bool playing;

		bool gameOver;
		//Game session state
		enum gameState{STATE_MAIN_MENU,STATE_OPTIONS,STATE_PREGAME,STATE_GAME};
		gameState state;
		//Mouse coordinates in a vec2f
		ofVec2f mousePos;
		ofVec2f rightMouseHeldPos;
		bool rightMouseDown;
		void drawFormationDirection();

		vector <unit> testers;
		int testerNumber;

		entitySystem entities;

		int aiClock;
		enum aiState{ASSESS, SELECT_START, SELECT_END, BOX_SELECT, ORDER_MOVE, ORDER_ATTACK, WAIT};
		aiState ai;
		ofVec2f topLeft, bottomRight;
		vector <ofVec2f> unitPositions;
		vector <ofVec2f> enemyPositions;

		bool shiftKeyDown;
		bool controlKeyDown;

		ofRectangle arena;

		//Listeners
		void playPressed();

        void audioRequested(float *output, int bufferSize, int nChannels);

        vector <float> notes;
        double signal;
};
