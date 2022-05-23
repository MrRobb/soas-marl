//
//  TrainTestThreadRandom.cpp
//  Thread for training/testing
//
//

#include <stdlib.h>

#include "TrainTestThreadRandom.hpp"
#include "ButtonsEnvironment.hpp"
#include "soas_marl.h"

void TrainTestThreadRandom::trainTestInit()
{
    // Initialize things before main loop
    printf("trainTestInit\n");

    if(timer_interval > 0)
    {
        timer.setPeriodicEvent(timer_interval);
        timer.reset();
    }
}

void TrainTestThreadRandom::trainTestLoop()
{
    for(uint32_t top_counter=0; top_counter<tester.getNumTimes(); top_counter++)
    {
        printf("Iteration %d\n", top_counter);
        // Reset everything. This includes deleting the agents that may have learned something.
        tester.restart();
        p_train_env->reset(true);
        p_test_env->reset(true);

        step = 0;
        epsilon = tester.learning_params.getInitialEpsilon();

        while( !tester.stopLearning() )
        {
            // Run Learning/Test episodes
            runEpisode();

            if(!isThreadRunning()) break;
        }

        if(!isThreadRunning()) break;

        printf("Finished iteration %d\n", top_counter);
    }
    // Need to check while(isThreadRunning()) periodically

    /* This code should limit the tick rate
            // May need a timer.reset() before the first one
            // Slow us down if the timer is enabled
            if(timer_interval > 0)
            {
                timer.waitNext();
            }
    */
}

void TrainTestThreadRandom::runEpisode()
{
    printf("Start episode\n");
    // Reset environment (but not agents)
    p_train_env->reset(false);
    for(uint32_t i=0; i<tester.learning_params.getMaxTsPerTask(); i++)
    {
        if(timer_interval > 0)
        {
            timer.waitNext();
        }
        
        tester.addStep();

        // Note: In DQPRM, need to add Q-Learning updates and reward machines

        // First, deal with agent movement
        vector<shared_ptr<Agent>> &agent_list = p_train_env->getAgents();
        for(uint32_t j=0; j<agent_list.size(); j++)
        {
            AgentAction_e action = agent_list[j]->getNextAction();
            agent_list[j]->performAction(action);
        }

        // Next, update environment after agents move
        p_train_env->updateEnvironment();

        // Last, broadcast a random event (no effect in TEAM_ENV)
        p_train_env->broadcastRandomEvent();

        // Check if we need to run a test step
        if( tester.testing_params.getTest() &&
            (0 == (tester.getCurrentStep() % tester.testing_params.getTestFreq()) ) )
        {
            runTest();
        }

        // Note: we're checking the environment for "solved" rather than each agent for "task complete"
        if( p_train_env->solved() )
        {
            printf("Goal!\n");
            // Reset environment (but not agents)
            p_train_env->reset(false);

            // If we have trained a minimum number of steps this episode and task is complete, exit
            if(tester.stopTask(i)) break;
        }

        // If we have hit the total training steps, exit
        if(tester.stopLearning()) break;
    }
    printf("End episode\n");

}

void TrainTestThreadRandom::runTest()
{
    printf("Start test\n");

    uint32_t test_steps = 0; // Number of test steps performed

    // This step used to keep track of how many training steps have been done so far
    step = tester.getCurrentStep();
    p_test_env->reset(false);
    // TODO: For learning agents, need to copy state from agents in training environment to
    //       these agents

    for(test_steps=0; test_steps<tester.testing_params.getNumSteps(); test_steps++)
    {
        if(timer_interval > 0)
        {
            timer.waitNext();
        }
        
        // Note: In DQPRM, make sure to set epsilon to -1 (or somehow mark exploit, not explore)

        // First, deal with agent movement
        vector<shared_ptr<Agent>> &agent_list = p_test_env->getAgents();
        for(uint32_t j=0; j<agent_list.size(); j++)
        {
            AgentAction_e action = agent_list[j]->getNextAction();
            agent_list[j]->performAction(action);
        }

        // Next, update environment after agents move
        p_test_env->updateEnvironment();

        // TODO: In DQPRM, need to check RM events/transitions
        // TODO: In DQPRM, need to check synchronization requirements on shared events
        // TODO: In DQPRM, may need to do part of update_agent (set is_task_complete maybe? The rest looks
        //       like it's taken care of already by our implementation


        // Note: we're checking the environment for "solved" rather than each agent for "task complete"
        if( p_test_env->solved() )
        {
            printf("Test goal!\n");
            break;
        }
    }

    // TODO: Save test results for plotting (test_steps contains number of required steps)

    printf("End test\n");

}

void TrainTestThreadRandom::trainTestPost()
{
    printf("trainTestPost\n");
    // Save agent list? Not sure why to bother with it
    // Plot results (or dump to file)
}

