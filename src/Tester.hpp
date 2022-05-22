//
//  Tester.hpp
//  Simple class to manage test/train loop
//
#pragma once

#include <stdio.h>  // For debugging

#include "ofMain.h"

class LearningParameters
{
public:
    // Default to numbers for buttons task. Setters to change if desired.
    LearningParameters() :
        gamma(0.9),
        alpha(0.8),
        T(50.0),
        initial_epsilon(0.0),
        max_timesteps_per_task(1000)
    {}

    void setGamma(float _gamma) { gamma = _gamma; };
    void setAlpha(float _alpha) { alpha = _alpha; };
    void setT(float _T) { T = _T; };
    void setInitialEpsilon(float _initial_epsilon) { initial_epsilon = _initial_epsilon; };
    void setMaxTsPerTask(float _max_timesteps_per_task) { max_timesteps_per_task = _max_timesteps_per_task; };

private:
    float gamma;
    float alpha;
    float T;
    float initial_epsilon;
    uint32_t max_timesteps_per_task;
};

class TestingParameters
{
public:
    // Default to numbers for buttons task. Setters to change if desired.
    TestingParameters() :
        test(true),
        test_freq(1000),
        num_steps(1000)
    {}

    void setTest(bool _test) { test = _test; }
    void setTestFreq(uint32_t _test_freq) { test_freq = _test_freq; }
    void setNumSteps(uint32_t _num_steps) { num_steps = _num_steps; }

private:
    bool test;
    uint32_t test_freq;
    uint32_t num_steps;
};


class Tester
{
public:
    LearningParameters learning_params;
    TestingParameters testing_params;

private:
    uint32_t step_unit;
    uint32_t total_steps;
    uint32_t min_steps;
    uint32_t num_times;
    uint32_t num_agents;
    // TODO: May need both team RM and list of RMs

    uint32_t current_step;

    // TODO: Maybe store results here? The way it's done in python looks more complicated than
    //       it needs to be, so maybe we can simplify it for our purposes. I think writing results
    //       to a file and then plotting with python might make the most sense.

public:
    Tester() :
        learning_params(),
        testing_params(),
        step_unit(1000),
        total_steps(250*1000),
        min_steps(1),
        num_times(10),
        num_agents(3),
        current_step(0)
    {};
    
    void setStepUnit(uint32_t _step_unit) { step_unit = _step_unit; }
    void setTotalSteps(uint32_t _total_steps) { total_steps = _total_steps; }
    void setMinSteps(uint32_t _min_steps) { min_steps = _min_steps; }
    void setNumTimes(uint32_t _num_times) { num_times = _num_times; }
    void setNumAgents(uint32_t _num_agents) { num_agents = _num_agents; }

    void restart() { current_step = 0; }
    void addStep() { current_step++; }
    uint32_t getCurrentStep() { return current_step; }
    bool stopLearning() { return total_steps <= current_step; }
    bool stopTask(uint32_t _step) { return min_steps <= _step; }


};
