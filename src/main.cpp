#include <stdlib.h>

//#include "ofMain.h"
#include "ofApp.h"

#include "soas_marl.h"

//========================================================================
int main( )
{
    ofGLFWWindowSettings settings;
    settings.resizable = false;
    settings.setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    ofCreateWindow(settings);

    // OF Init stuff
    //ofSetVerticalSync(false);  // Don't want since we double-buffer and draw snapshots of env
    ofSetFrameRate(FRAME_RATE);
    //ofSetBackgroundColorHex(ofHexToInt("0D1B1E"));
    ofSetBackgroundColorHex(ofHexToInt("000000"));  // Ideally, should match the wall colors in the env

    // Other init stuff before starting app (singleton instantiation, global data, etc.)
    time_t t;
    srand(time(&t));

    // Run app
    ofRunApp(new ofApp());
}
