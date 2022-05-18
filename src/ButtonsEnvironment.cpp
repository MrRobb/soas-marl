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
    // This routine should draw to an ofFBO object that is passed into it
    _fbo.begin();

    // Step 1: We'll put some dummy code here just to draw something and make sure we're aligned right
    //         without worrying about threading issues

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

    _fbo.end();

    // Step 2: We'll get a snapshot of the environment state from a thread-safe method since another thread
    //         will be ticking the environment. We do not want the snapshot access to block, so we'll get the
    //         last valid environment state rather than wait for the current update to complete.
}

