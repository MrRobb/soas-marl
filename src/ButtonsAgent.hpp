//
//  ButtonsAgent.hpp
//  Agent for the buttons task
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

// Forward-declaration of environment
class ButtonsEnvironment;


class ButtonsAgent
{
public:
    ButtonsEnvironment &env;
    ofVec2f location;

private:
    // Probably stuff here like Q-Table

public:
	ButtonsAgent(ButtonsEnvironment &_env) : env(_env) {}  // Loc should be 0,0
	ButtonsAgent(ButtonsEnvironment &_env, ofVec2f _init_loc) :
        env(_env),
        location(_init_loc)
    {}

    void performAction(AgentAction_e _action);
    // TODO: Maybe add a function to return a list of valid actions?
};
