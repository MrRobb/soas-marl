//
//  TrainTestThreadDqprm.hpp
//  Thread for training/testing
//  Random policy has no training, so this is just a testing loop
//
#pragma once

#include <stdio.h>  // For debugging

#include "ofMain.h"
#include "Tester.hpp"

// Forward-declaration of ButtonsEnvironment
class ButtonsEnvironment;

class TrainTestThreadDqprm : public ofThread
{
private:
    // Settings like tick rate, etc.
    ofTimer timer;
    uint64_t timer_interval;            // Nanoseconds between ticks, 0 to disable
    ButtonsEnvironment *p_train_env[3];  // List of training environments
    ButtonsEnvironment *p_test_env;     // Reference to test environment
    Tester   tester;
    uint32_t step;
    float    epsilon;

public:
    TrainTestThreadDqprm() :
        timer(),
        timer_interval(0),  // Disable timer by default
        p_train_env{ nullptr, nullptr, nullptr },
        p_test_env(nullptr),
        tester(),
        step(0),
        epsilon(0)
    {
    }

    void registerTrainEnv1(ButtonsEnvironment *_p_env) { p_train_env[0] = _p_env; }
    void registerTrainEnv2(ButtonsEnvironment *_p_env) { p_train_env[1] = _p_env; }
    void registerTrainEnv3(ButtonsEnvironment *_p_env) { p_train_env[2] = _p_env; }
    void registerTestEnv(ButtonsEnvironment *_p_env) { p_test_env = _p_env; }

    void setTimerInterval(uint64_t _interval)
    {
        timer_interval = _interval;
    }

    void threadedFunction()
    {
        printf("Starting...\n");

        if( (nullptr == p_train_env[0]) || (nullptr == p_train_env[1]) || 
            (nullptr == p_train_env[2]) || (nullptr == p_test_env    )  )
        {
            printf("Error! Environments not registered!\n");
            return;
        }

        trainTestInit();

        trainTestLoop();  // Start checking isThreadRunning inside this function for early exit

        if(isThreadRunning())
        {
            trainTestPost();
        }
    }

    // Fill in these functions to run training loop
    void trainTestInit();
    void trainTestLoop();
    void trainTestPost();

    void runEpisode();
    void runTest();

};
