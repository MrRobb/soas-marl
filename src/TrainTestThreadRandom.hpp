//
//  TrainTestThreadRandom.hpp
//  Thread for training/testing
//  Random policy has no training, so this is just a testing loop
//
#pragma once

#include <stdio.h>  // For debugging

#include "ofMain.h"

// Forward-declaration of ButtonsEnvironment
class ButtonsEnvironment;

class TrainTestThreadRandom : public ofThread
{
private:
    // Settings like tick rate, etc.
    ofTimer timer;
    uint64_t timer_interval;        // Nanoseconds between ticks, 0 to disable
    ButtonsEnvironment *p_train_env;  // Reference to train environment
    ButtonsEnvironment *p_test_env;   // Reference to test environment
    uint32_t counter;
    uint32_t limit;

public:
    TrainTestThreadRandom() :
        timer(),
        timer_interval(0),  // Disable timer by default
        p_train_env(nullptr),
        p_test_env(nullptr),
        counter(0),
        limit(100)
    {
    }

    void registerTrainEnv(ButtonsEnvironment *_p_env) { p_train_env = _p_env; }
    void registerTestEnv(ButtonsEnvironment *_p_env) { p_test_env = _p_env; }

    void setTimerInterval(uint64_t _interval)
    {
        timer_interval = _interval;
    }

    // Just the boilerplate code to limit the tick rate if desired. The "real" code should
    // go in trainTestLoop(). There are also hooks for trainTestInit() and trainTestPost()
    // for setup/cleanup, respectively. Add member variables to this class to control
    // state between iterations.
    void threadedFunction()
    {
        bool done = false;
        printf("Starting...\n");

        if( (nullptr == p_train_env) || (nullptr == p_test_env) )
        {
            printf("Error! Environments not registered!\n");
            return;
        }

        trainTestInit();

        // Initialize things before main loop
        if(timer_interval > 0)
        {
            timer.setPeriodicEvent(timer_interval);
            timer.reset();
        }

        while(isThreadRunning())
        {
            // Slow us down if the timer is enabled
            if(timer_interval > 0)
            {
                timer.waitNext();
            }

            // Run trainTestLoop until done
            done = trainTestLoop();
            if( done ) break;
        }

        trainTestPost();
    }

    // Fill in these functions to 
    void trainTestInit();
    bool trainTestLoop();
    void trainTestPost();

};
