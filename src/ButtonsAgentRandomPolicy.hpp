//
//  ButtonsAgentRandomPolicy.hpp
//  Agent for the buttons task that implements a random action policy
//
#pragma once

#include <stdio.h>  // For debugging

#include "ofMain.h"

#include "Agent.hpp"

// Forward-declaration of environment
class ButtonsEnvironment;


class ButtonsAgentRandomPolicy : public Agent
{
private:
    // Probably stuff here like Q-Table

public:
	ButtonsAgentRandomPolicy(ButtonsEnvironment &_env) : Agent(_env) {}
	ButtonsAgentRandomPolicy(ButtonsEnvironment &_env, ofVec2f _init_loc) :
        Agent(_env, _init_loc)
    {}

    AgentAction_e getNextAction();
};
