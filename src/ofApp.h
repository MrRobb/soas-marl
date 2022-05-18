#pragma once

#include "ofMain.h"
#include <vector>
#include "ButtonsEnvironment.hpp"

class ofApp : public ofBaseApp{

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
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

    private:
        static ofVec2f getEnvCoords(int index, int rows, int cols);

private:
    // Future: Make this an abstract Environment class. Not needed for this project.
    vector<ButtonsEnvironment> envs;  // Environment objects
    vector<ofVec2f> env_locs;         // Location where to draw each environment
    vector<ofFbo> env_fbos;           // Frame buffers holding the rendered environment
};
