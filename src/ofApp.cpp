// OF includes
#include "ofApp.h"

// OpenGL include (needs to come after OF includes)
#include "GLFW/glfw3.h"


// Local includes
#include "soas_marl.h"
#include "SparseRewardMachine.hpp"

ofApp::ofApp() :
    envs()
{
}

//--------------------------------------------------------------
void ofApp::setup()
{
    // Config - how many environments to draw (just one now, but keep flexible)
    int total_envs = NUM_ENV_ROW*NUM_ENV_COL;
    //assert(total_envs == 1); // Since we're only supporting one for now

    // Initialize environment objects
    envs = vector<ButtonsEnvironment> (total_envs);
    env_locs = vector<ofVec2f> (total_envs);
    env_fbos = vector<ofFbo> (total_envs);
    ButtonsEnvironmentConfig_e cfgs[4] = 
        { AGENT1_ENV, AGENT2_ENV, AGENT3_ENV, TEAM_ENV };
    for (int i = 0; i < total_envs; i++)
    {
        envs[i].setConfig(cfgs[i]);
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
        // Run Random Policy test

        // Only need to use the first two environments for this
        ButtonsEnvironmentConfig_e cfgs[4] = 
            { TEAM_ENV, TEAM_ENV, EMPTY_ENV, EMPTY_ENV };
        for (int i = 0; i < 4; i++)
        {
            envs[i].setConfig(cfgs[i]);
        }
        train_test_random_policy.registerTrainEnv(&envs[0]);
        train_test_random_policy.registerTestEnv(&envs[1]);

        train_test_random_policy.setTimerInterval(10000000); // 100 Hz
        //train_test_random_policy.setTimerInterval(1000000); // 1000 Hz
        //train_test_random_policy.setTimerInterval(0); // No limit
        train_test_random_policy.startThread();
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

    // G for Go
    //  Need to OR with 0x20 for lower-case
    if ((key == GLFW_KEY_G)||(key == (GLFW_KEY_G|0x20))) {
        // Note: This code to be deprecated soon. It makes more sense to have a single thread
        //       control multiple environments than a single thread per-environment, the way
        //       the train/test loop is set up.
        for(int i=0; i<4; i++)
        {
            //envs[i].go(500000000, 20);  // Go for 20 ticks at 0.5 sec/tick
            //envs[i].go(100000000, 200);  // Go for 200 ticks at 0.1 sec/tick
            envs[i].go(10000000, 200);  // Go for 200 ticks at 100 Hz
            //envs[i].go(0, 200);  // Go for 200 ticks as fast as CPU will allow
        }
    }

    // T for Test
    //  Need to OR with 0x20 for lower-case
    if ((key == GLFW_KEY_T)||(key == (GLFW_KEY_T|0x20))) {
        // Reward machine unit test
        printf("Running unit test.\n");

        printf("Buttons 1\n");
        SparseRewardMachine rm1(std::stringstream(SparseRewardMachine::getRmBuf(RM_BUTTONS_1)));
        printf("Buttons 2\n");
        SparseRewardMachine rm2(std::stringstream(SparseRewardMachine::getRmBuf(RM_BUTTONS_2)));
        printf("Buttons 3\n");
        SparseRewardMachine rm3(std::stringstream(SparseRewardMachine::getRmBuf(RM_BUTTONS_3)));
        printf("Team Buttons\n");
        SparseRewardMachine rmteam(std::stringstream(SparseRewardMachine::getRmBuf(RM_BUTTONS_TEAM)));
        printf("Done\n");
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
