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


class ButtonsAgent
{
public:
    ofVec2f location;

private:
    // Probably stuff here like Q-Table

public:
	ButtonsAgent() {}  // Loc should be 0,0
	ButtonsAgent(ofVec2f _init_loc) { location = _init_loc; }
};
