//
//  TrainTestThreadDqprm.cpp
//  Thread for training/testing
//
//

#include <stdlib.h>

#include "TrainTestThreadDqprm.hpp"
#include "ButtonsEnvironment.hpp"
#include "QAgent.hpp"
#include "soas_marl.h"

void TrainTestThreadDqprm::trainTestInit()
{
    // Initialize things before main loop
    printf("trainTestInit\n");

    if(timer_interval > 0)
    {
        timer.setPeriodicEvent(timer_interval);
        timer.reset();
    }
}

void TrainTestThreadDqprm::trainTestLoop()
{
    for(uint32_t top_counter=0; top_counter<tester.getNumTimes(); top_counter++)
    {
        printf("Iteration %d\n", top_counter);
        // Reset everything. This includes deleting the agents that may have learned something.
        tester.restart();
        for(int i=0; i<3; i++)
        {
            p_train_env[i]->reset(true);
        }
        p_test_env->reset(true);

        step = 0;
        epsilon = tester.learning_params.getInitialEpsilon();

        while( !tester.stopLearning() )
        {
            // Run Learning/Test episodes
            runEpisode(epsilon);

            if(!isThreadRunning()) break;
        }

        if(!isThreadRunning()) break;

        printf("Finished iteration %d\n", top_counter);
    }
}

void TrainTestThreadDqprm::runEpisode(double _epsilon)
{
    printf("Start episode\n");
    // Reset environment (but not agents)
    p_train_env[0]->reset(false);
    p_train_env[1]->reset(false);
    p_train_env[2]->reset(false);
    for(uint32_t i=0; i<tester.learning_params.getMaxTsPerTask(); i++)
    {
        printf("Wait for interval\n");
        if(timer_interval > 0)
        {
            timer.waitNext();
        }
        
        tester.addStep();

        // Run for one tick in each training environment
        for(uint32_t j=0; j<3; j++)
        {
            printf("Updating environment: %d\n", j);
            // Only need to run the environments that have not solved their task
            if(!p_train_env[j]->solved())
            {
                // First, deal with agent movement (should be one agent per environment)
                vector<shared_ptr<Agent>> &agent_list = p_train_env[j]->getAgents();

                for(uint32_t k=0; k<agent_list.size(); k++)
                {
                    printf("Updating agent: %d\n", k);
                    Agent *agent = agent_list[k].get();
                    QAgent *qagent = dynamic_cast<QAgent *>(agent);

                    if(!qagent->getTaskComplete())
                    {
                        MachineState current_u = qagent->getCurrentU();
                        MachineState orig_state = qagent->getAgentState();
                        std::vector<AgentAction_e> actions {};
                        actions.push_back(qagent->get_next_action(_epsilon, tester.learning_params));
                        
                        // outputs: reward, labels, new_state
                        Reward reward=0;
                        std::vector<Event> labels;
                        std::vector<MachineState> new_state;
                        p_train_env[j]->environmentStep(actions, reward, labels, new_state);

                        qagent->update_agent(new_state[k], actions[k], reward, labels, tester.learning_params);

                        // TODO: Need to update Q function for other RM transitions here
                        // U=qagent->getUSet()
                        // Need to either get rm.T or method that checks if u is in T (probably better)
                        for(auto u : qagent->getUSet())
                        {
                            if((u!=current_u)&&(!qagent->isUinT(u)))
                            {
                                labels.clear(); // Reuse labels, reward
                                p_train_env[j]->getMDPLabel(new_state, u, labels);
                                reward=0;
                                MachineState u_temp = u;
                                MachineState u2 = u;
                                for( auto e : labels )
                                {
                                    u2 = qagent->getRM().get_next_state(u_temp, e);
                                    reward = reward + qagent->getRM().get_reward(u_temp, u2);
                                    u_temp = u2;
                                }
                                qagent->update_q_function(orig_state, new_state[k], u, u2, actions[k], reward, tester.learning_params );
                            }
                        }
                    }
                    else
                    {
                        printf("Task complete!\n");
                    }
                }

                // We still need to do this.
                printf("Updating environment after agents move\n");
                // Next, update environment after agents move
                p_train_env[j]->updateEnvironment();

                // I think this is taken care of now in the MDP label function
                /*
                printf("Broadcast random event\n");
                // Last, broadcast a random event (no effect in TEAM_ENV)
                p_train_env[j]->broadcastRandomEvent();
                */
            }
        }

        printf("Check if we need to run a test step\n");
        // Check if we need to run a test step
        if( tester.testing_params.getTest() &&
            (0 == (tester.getCurrentStep() % tester.testing_params.getTestFreq()) ) )
        {
            runTest();
        }

        
        // Note: we're checking the environment for "solved" rather than each agent for "task complete"
        if( p_train_env[0]->solved() && p_train_env[1]->solved() && p_train_env[2]->solved() )
        {
            printf("All training environments reached goal!\n");
            // Reset environment (but not agents)
            p_train_env[0]->reset(false);
            p_train_env[1]->reset(false);
            p_train_env[2]->reset(false);

            // If we have trained a minimum number of steps this episode and task is complete, exit
            if(tester.stopTask(i)) break;
        }

        // If we have hit the total training steps, exit
        if(tester.stopLearning()) break;
    }
    printf("End episode\n");

}

void TrainTestThreadDqprm::runTest()
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
        printf("Test timestep: %d\n", test_steps);
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

void TrainTestThreadDqprm::trainTestPost()
{
    printf("trainTestPost\n");
    // Save agent list? Not sure why to bother with it
    // Plot results (or dump to file)
}

