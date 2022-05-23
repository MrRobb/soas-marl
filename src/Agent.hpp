//
//  Agent.hpp
//  Agent interface class and generic agent things like movements
//
#pragma once

#include <stdio.h>  // For debugging

#include "ofMain.h"

typedef enum
{
    MOVE_UP     = 0,
    MOVE_DOWN   = 1,
    MOVE_LEFT   = 2,
    MOVE_RIGHT  = 3,
    STAY        = 4,
    NUM_ACTIONS = 5
} AgentAction_e;

// Forward-declaration of ButtonsEnvironment
class ButtonsEnvironment;

class Agent
{
protected:
    ButtonsEnvironment& env;
    ofVec2f location;

public:
    Agent(ButtonsEnvironment &_env) : env(_env) {}  // location defaults to 0, 0
    Agent(ButtonsEnvironment &_env, ofVec2f _init_loc) :
        env(_env),
        location(_init_loc)
    {}
    
    // These are the interface functions to override
	virtual AgentAction_e getNextAction() { return STAY; }
    // End interface functions

    void performAction(AgentAction_e _action);
    ofVec2f getLocation() { return location; }
    void setLocation(ofVec2f _loc) { location = _loc; }  // Should only be called to initialize environment

    int getAgentState();  // Helper function for Q-Learning state

    static ofVec2f getActionVector(AgentAction_e _action);
};
