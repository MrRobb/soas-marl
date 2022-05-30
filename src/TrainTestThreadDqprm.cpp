//
//  TrainTestThreadDqprm.cpp
//  Thread for training/testing
//
//

#include <stdlib.h>
#include <iostream>

#include "TrainTestThreadDqprm.hpp"
#include "ButtonsEnvironment.hpp"
#include "QAgent.hpp"
#include "soas_marl.h"

// Set intersection for unordered set and event label list.
// Should only need locally to this file. If not, find a nice header for it.
void project_labels(std::unordered_set<Event> &_s, std::vector<Event> &_l, std::vector<Event> &_p)
{
    _p.clear();

    for(Event l : _l)
    {
        if(_s.find(l) != _s.end())
        {
            _p.push_back(l);
        }
    }
}

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

                        // Update Q function for other RM transitions here
                        // This is a little strange, but seems to be one of the major differences between
                        // standard multi-agent Q-Learning and DQPRM
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

    // Now, reset the environment and re-create the agents fresh. Then, copy the 
    p_test_env->reset(true);

    // Copy the knowledge from the trained agents to the test agents
    vector<shared_ptr<Agent>> &test_agent_list = p_test_env->getAgents();

    vector<QAgent *> qagent_list;
    for(int i=0; i<3; i++)
    {
        vector<shared_ptr<Agent>> &train_agent_list = p_train_env[i]->getAgents();
        Agent *train_agent = train_agent_list[0].get();
        QAgent *train_qagent = dynamic_cast<QAgent *>(train_agent);

        Agent *test_agent = test_agent_list[i].get();
        QAgent *test_qagent = dynamic_cast<QAgent *>(test_agent);

        test_qagent->absorbKnowledge(*train_qagent);

        // While we're at it, let's save the QAgents in a vector
        qagent_list.push_back(test_qagent);
    }


    for(test_steps=0; test_steps<tester.testing_params.getNumSteps(); test_steps++)
    {
        printf("Test timestep: %d\n", test_steps);
        if(timer_interval > 0)
        {
            timer.waitNext();
        }
        
        // Choose an action
        std::vector<AgentAction_e> actions {};
        for(uint32_t i=0; i<qagent_list.size(); i++)
        {
            // Epsilon set to -1 (exploit, not explore)
            actions.push_back(qagent_list[i]->get_next_action(-1.0, tester.learning_params));
        }

        // outputs: reward, labels, new_state
        Reward reward=0;
        std::vector<Event> labels;
        std::vector<MachineState> new_state;
        p_test_env->environmentStep(actions, reward, labels, new_state);
        // No real need to keep track of reward. We'll just track steps to completion.

        // Here, we need to convert the events generated by the environment RM into local
        // events for each agent's RM. Use the "projection" concept.
        std::vector<std::vector<Event>> projected_l;
        for(uint32_t i=0; i<qagent_list.size(); i++)
        {
            std::vector<Event> projected_events;
            project_labels(qagent_list[i]->getLocalEventSet(), labels, projected_events);
            // Only store it if there is a transition from current state on this event
            if(!(qagent_list[i]->is_local_event_available(projected_events)) ) {
                projected_events.clear();
            }
            projected_l.push_back(projected_events);
        }

        // Synchronize shared events
        for(uint32_t i=0; i<qagent_list.size(); i++)
        {
            if( projected_l[i].size() > 0 )
            {
                for( Event e : projected_l[i] )
                {
                    for(uint32_t j=0; j<qagent_list.size(); j++)
                    {
                        if(qagent_list[i]->getLocalEventSet().find(e) != qagent_list[i]->getLocalEventSet().end() )
                        {
                            // TODO: This isn't right. Need an element-by-element comparison.
                            if(projected_l[j]!=projected_l[j])
                            {
                                projected_l[i] = std::vector<Event>();
                            }
                        }
                    }
                }
            }

            // Now update the agents
            qagent_list[i]->update_agent(new_state[i], actions[i], reward, labels, tester.learning_params, false);
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

    tester.saveResult(test_steps);

    printf("End test\n");

}

void TrainTestThreadDqprm::trainTestPost()
{
    printf("trainTestPost\n");
    // Save agent list? Not sure why to bother with it
    // Plot results (or dump to file)
}

