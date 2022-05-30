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

    void setGamma(double _gamma) { gamma = _gamma; };
    void setAlpha(double _alpha) { alpha = _alpha; };
    void setT(double _T) { T = _T; };
    void setInitialEpsilon(double _initial_epsilon) { initial_epsilon = _initial_epsilon; };
    void setMaxTsPerTask(double _max_timesteps_per_task) { max_timesteps_per_task = _max_timesteps_per_task; };

    double getGamma() { return gamma; }
    double getAlpha() { return alpha; }
    double getT() { return T; }
    double getInitialEpsilon() { return initial_epsilon; }
    uint32_t getMaxTsPerTask() {return max_timesteps_per_task; }

private:
    double gamma;
    double alpha;
    double T;
    double initial_epsilon;
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

    bool getTest() { return test; }
    uint32_t getTestFreq() { return test_freq; }
    uint32_t getNumSteps() { return num_steps; }

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
    ofstream results_file;

public:
    Tester() :
        learning_params(),
        testing_params(),
        step_unit(1000),
        total_steps(40*1000),
        min_steps(1),
        num_times(10),
        num_agents(3),
        current_step(0)
    {
        results_file.open("results.csv");
        results_file << "Training" << "," << "Testing" << endl;
    };
    
    // Setters
    void setStepUnit(uint32_t _step_unit) { step_unit = _step_unit; }
    void setTotalSteps(uint32_t _total_steps) { total_steps = _total_steps; }
    void setMinSteps(uint32_t _min_steps) { min_steps = _min_steps; }
    void setNumTimes(uint32_t _num_times) { num_times = _num_times; }
    void setNumAgents(uint32_t _num_agents) { num_agents = _num_agents; }

    // Getters - not all of them seem necessary
    uint32_t getStepUnit() { return step_unit; }
    uint32_t getTotalSteps() { return total_steps; }
    uint32_t getMinSteps() { return min_steps; }
    uint32_t getNumTimes() { return num_times; }
    uint32_t getNumAgents() { return num_agents; }

    void restart() { current_step = 0; }
    void addStep() { current_step++; }
    uint32_t getCurrentStep() { return current_step; }
    bool stopLearning() { return total_steps <= current_step; }
    bool stopTask(uint32_t _step) { return min_steps <= _step; }

    void saveResult(uint32_t num_steps) {
        results_file << current_step << "," << num_steps << endl;
    }
};
