//
//  Agent.cpp
//  Agent interface class
//
//

#include "Agent.hpp"
#include "ButtonsEnvironment.hpp"
#include "soas_marl.h"

// Lookup table in scope of this file only
static ofVec2f ActionLut[NUM_ACTIONS] =
{
    { ofVec2f( 0,-1) }, // MOVE_UP
    { ofVec2f( 0, 1) }, // MOVE_DOWN
    { ofVec2f(-1, 0) }, // MOVE_LEFT
    { ofVec2f( 1, 0) }, // MOVE_RIGHT
    { ofVec2f( 0, 0) }  // STAY
};

ofVec2f Agent::getActionVector(AgentAction_e _action)
{
    return ActionLut[_action];
}

void Agent::performAction(AgentAction_e _action)
{
    ofVec2f new_loc = location + getActionVector(_action);

    // Make sure the action is legal overall (not moving outside of environment)
    if( (new_loc.x < 0) || (new_loc.x > 9) ) return;
    if( (new_loc.y < 0) || (new_loc.y > 9) ) return;

    // Make sure the action is legal in the current environment state
    if(!env.isOpen(new_loc)) return;

    // If we made it here, it's safe. Apply the action.
    location = new_loc;
}

int Agent::getAgentState()
{
    // Just a helper to convert the agent's position into a single number
    // in order to encode the possible 100 agent locations
    return (location.y*GRID_C)+location.x;
}
