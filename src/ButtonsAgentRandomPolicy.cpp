//
//  ButtonsAgentRandomPolicy.cpp
//  Implementation of buttons agent
//
//

#include "ButtonsAgentRandomPolicy.hpp"
#include "ButtonsEnvironment.hpp"
#include "soas_marl.h"

AgentAction_e ButtonsAgentRandomPolicy::getNextAction()
{
    // Picks a random action
    AgentAction_e action = (AgentAction_e)(rand() % NUM_ACTIONS);
    return action;
}
