// OF includes
#include "ofApp.h"

// OpenGL include (needs to come after OF includes)
#include "GLFW/glfw3.h"


// Local includes
#include "soas_marl.h"


//--------------------------------------------------------------
void ofApp::setup()
{
    // Config - how many environments to draw (just one now, but keep flexible)
    int total_envs = NUM_ENV_ROW*NUM_ENV_COL;
    assert(total_envs == 1); // Since we're only supporting one for now

    // Initialize environment objects
    envs = vector<ButtonsEnvironment> (total_envs);
    env_locs = vector<ofVec2f> (total_envs);
    env_fbos = vector<ofFbo> (total_envs);
    for (int i = 0; i < total_envs; i++)
    {
        envs[i] = ButtonsEnvironment();
        env_locs[i] = getEnvCoords(i, NUM_ENV_ROW, NUM_ENV_COL);
        env_fbos[i] = ofFbo();
        env_fbos[i].allocate(ENV_WIDTH, ENV_HEIGHT, GL_RGBA);
        env_fbos[i].begin();
        ofClear(255, 255, 255, 255);
        env_fbos[i].end();
    }
}

//--------------------------------------------------------------
void ofApp::update()
{
    // TODO: Move this to own thread. Shouldn't be dependent on framerate, etc.
    // Compute
    for (auto& env : envs) {
        env.tick();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    int num_envs = envs.size();
    for (int i=0; i<num_envs; i++)
    {
        env_locs[i];
        envs[i].render(env_fbos[i]);   // Render environment to FBO
        env_fbos[i].draw(env_locs[i]); // Draw to window
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == GLFW_KEY_1) {
    }
    if (key == GLFW_KEY_2) {
    }
    if (key == GLFW_KEY_3) {
    }
    if (key == GLFW_KEY_4) {
    }
    if (key == GLFW_KEY_5) {
    }
    if (key == GLFW_KEY_6) {
    }
    if (key == GLFW_KEY_7) {
    }
    if (key == GLFW_KEY_8) {
    }
    if (key == GLFW_KEY_9) {
    }
    if (key == GLFW_KEY_0) {
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y )
{
	//old = target;
	//target = ofVec2f(mouseX, mouseY);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
/*
	if (button == OF_MOUSE_BUTTON_1)
		v.push_back(Specimen(x, y));
	else
		v.pop_back();
*/
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
/*
	if (button == OF_MOUSE_BUTTON_1)
		v.push_back(Specimen(x, y));
	else
		v.pop_back();
*/
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{

}

//--------------------------------------------------------------
ofVec2f ofApp::getEnvCoords(int index, int rows, int cols)
{
    assert(index < (rows*cols));
    int row = index / cols;
    int col = index % cols;
    ofVec2f coords = ofVec2f(col*ENV_HEIGHT_PLUS_MARGIN, row*ENV_WIDTH_PLUS_MARGIN);  // x, y -> col, row
    coords.x += MARGIN;
    coords.y += MARGIN;
    return coords;
}
