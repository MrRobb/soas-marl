//
//  ButtonsEnvironment.cpp
//  Implementation of buttons environment
//
//

#include <stdlib.h>

#include "ButtonsEnvironment.hpp"
#include "soas_marl.h"

void ButtonsEnvironmentThread::threadedFunction()
{
    // Initialize things before main loop
    if(timer_interval > 0)
    {
        timer.setPeriodicEvent(timer_interval);
        timer.reset();
    }
    ticks = 0;

    while(isThreadRunning())
    {
        // Slow us down if the timer is enabled
        if(timer_interval > 0)
        {
            timer.waitNext();
        }

        printf("Tick %d\n", ticks);  // TODO: Delete this eventually
        // This code calls the environment to update itself to the next state
        env.tick();

        // Exit when we have run enough times
        ticks++;
        if(ticks >= tick_limit)
        {
            break;
        }
    }
}

ButtonsEnvironment::ButtonsEnvironment() :
    env_thread(*this)
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

    // Initialize agents - TODO: May also want environments with just a single agent for training
    agents.reserve(3);
    agents.emplace_back(ButtonsAgent(*this, ofVec2f(0,0)));
    agents.emplace_back(ButtonsAgent(*this, ofVec2f(5,0)));
    agents.emplace_back(ButtonsAgent(*this, ofVec2f(8,0)));

    yellow_button_loc = ofVec2f(2,0);
    red_button_loc = ofVec2f(9,6);
    green_button_loc = ofVec2f(6,5);
    goal_loc = ofVec2f(8,8);
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
    // TODO: Add code here to have each agent choose their next move and update the environment accordingly

    // Example: make each agent pick a random action each tick
    for(uint32_t i=0; i<agents.size(); i++)
    {
        AgentAction_e action = (AgentAction_e)(rand() % NUM_ACTIONS);
        agents[i].performAction(action);
    }

    // Then, after applying movement, check for events (like button presses)
    updateEnvironment();
}

void ButtonsEnvironment::updateEnvironment()
{
    for(uint32_t i=0; i<agents.size(); i++)
    {
        if( agents[i].location == yellow_button_loc )
        {
            enableYellowBarrier(false);
        }
        if( agents[i].location == green_button_loc )
        {
            enableGreenBarrier(false);
        }
        if( agents[i].location == red_button_loc )
        {
            // Need to check for a second agent on it
            for(uint32_t j=i+1; j<agents.size(); j++)
            {
                if( agents[j].location == red_button_loc )
                {
                    enableRedBarrier(false);
                }
            }
        }
    }
}

void ButtonsEnvironment::go(uint64_t _timer_interval, int _tick_limit)
{
    env_thread.setTimerInterval(_timer_interval);
    env_thread.setTickLimit(_tick_limit);
    env_thread.startThread();
}

bool ButtonsEnvironment::isOpen(ofVec2f _loc)
{
    CellType_e cell_type = env_state[(int)_loc.x][(int)_loc.y];
    bool rval = false;

    switch( cell_type )
    {
        case EMPTY: // Fall through by design
        case RED_BUTTON: // Fall through by design
        case YELLOW_BUTTON: // Fall through by design
        case GREEN_BUTTON: // Fall through by design
        case GOAL:
            rval = true;
            break;
        default:
            rval = false;
            break;
    }
    return rval;
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
