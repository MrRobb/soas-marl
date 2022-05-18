//
//  ButtonsEnvironment.cpp
//  Implementation of buttons environment
//
//

#include "ButtonsEnvironment.hpp"
#include "soas_marl.h"

ButtonsEnvironment::ButtonsEnvironment()
{
    // Set up map
    env_state[3][0] = env_state[3][1] = env_state[3][2] = env_state[3][3] = env_state[3][4] =
                      env_state[3][5] = env_state[3][6] = env_state[3][7] = WALL;
    env_state[4][7] = env_state[5][7] = env_state[6][7] = env_state[7][7] = env_state[8][7] =
                      env_state[9][7] = WALL;
    env_state[7][0] = env_state[7][1] = env_state[7][2] = env_state[7][3] = env_state[7][4] = WALL;
    enableGreenBarrier(true);
    enableYellowBarrier(true);
    enableRedBarrier(true);
    env_state[2][0] = YELLOW_BUTTON;
    env_state[6][5] = GREEN_BUTTON;
    env_state[9][6] = RED_BUTTON;
    env_state[8][8] = GOAL;

    // Initialize agents
    agents = vector<ButtonsAgent>(3);
    agents[0].location = ofVec2f(0,0);
    agents[1].location = ofVec2f(5,0);
    agents[2].location = ofVec2f(8,0);
}

void ButtonsEnvironment::enableGreenBarrier(bool _enable)
{
    CellType_e value = _enable ? GREEN_BARRIER : EMPTY;
    env_state[8][2] = env_state[8][3] = env_state[9][2] = env_state[9][3] = value;
}
void ButtonsEnvironment::enableYellowBarrier(bool _enable)
{
    CellType_e value = _enable ? YELLOW_BARRIER : EMPTY;
    env_state[4][2] = env_state[4][3] = env_state[5][2] = env_state[5][3] = env_state[6][2] =
                      env_state[6][3] = value;
}
void ButtonsEnvironment::enableRedBarrier(bool _enable)
{
    CellType_e value = _enable ? RED_BARRIER : EMPTY;
    env_state[5][8] = env_state[5][9] = env_state[6][8] = env_state[6][9] = env_state[7][8] =
                      env_state[7][9] = value;
}

void ButtonsEnvironment::tick()
{
    // Run for one cycle
}

void ButtonsEnvironment::render(ofFbo _fbo)
{
    // This routine should draw to an ofFBO object that is passed into it. The caller
    // will then draw one or more of these buffers to the screen.
    _fbo.begin();

    // TODO: Get thread-safe snapshot of environment state since another thread will be ticking the
    //       environment. We do not want the snapshot access to block, so we'll get the last valid
    //       environment state rather than wait for the current update to complete.
    // The only part that should change is using an object to access env_state instead of env_state directly.

    // Render background (walls, empty space, barriers, buttons)
    for(int i=0; i<10; i++)
    {
        for(int j=0; j<10; j++)
        {
            float x = i*SQUARE_LEN;
            float y = j*SQUARE_LEN;
            // Draw background color for all cells
            CellType_e type = env_state[i][j];
            ofSetHexColor(cell_info.data[type].bgcolor);
            ofDrawRectangle(x,y,SQUARE_LEN,SQUARE_LEN);

            // Draw buttons if needed
            if( cell_info.data[type].has_circle )
            {
                ofSetHexColor(cell_info.data[type].circle_color);
                ofDrawCircle(x+(SQUARE_LEN/2),y+(SQUARE_LEN/2),BUTTON_RADIUS);
            }
        }
    }

    // Render foreground (agents)
    for(auto& a : agents)
    {
        drawAgent(_fbo, a);
    }

    _fbo.end();

}

void ButtonsEnvironment::drawAgent(ofFbo _fbo, ButtonsAgent &_a)
{
    float x = _a.location.x*SQUARE_LEN;
    float y = _a.location.y*SQUARE_LEN;

    ofSetHexColor(PURPLE);
    ofDrawTriangle(x+12, y+2, x+2, y+22, x+22, y+22);

}
