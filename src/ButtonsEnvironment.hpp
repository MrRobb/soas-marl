//
//  ButtonsEnvironment.hpp
//  Environment for the buttons team task
//
#pragma once

#include <stdio.h>  // For debugging

#include "ofMain.h"
#include "Agent.hpp"
#include "ButtonsAgentRandomPolicy.hpp"
#include "SparseRewardMachine.hpp"

#define WHITE  0xffffff
#define BLACK  0x000000
#define RED    0xff0000
#define GREEN  0x00ff00
#define BLUE   0x0000ff
#define YELLOW 0xffff00
#define PURPLE 0xff33ff

typedef enum
{
    EMPTY_ENV = 0,
    TEAM_ENV = 1,
    AGENT1_ENV = 2,
    AGENT2_ENV = 3,
    AGENT3_ENV = 4,
    NUM_ENV_CONFIGS
} ButtonsEnvironmentConfig_e;

typedef enum
{
    EMPTY = 0,
    WALL  = 1,
    RED_BARRIER = 2,
    YELLOW_BARRIER = 3,
    GREEN_BARRIER = 4,
    RED_BUTTON = 5,
    YELLOW_BUTTON = 6,
    GREEN_BUTTON = 7,
    GOAL = 8,
    NUM_CELL_TYPE_INFO = 9
} CellType_e;

struct CellInfo
{
    bool traversable;
    int  bgcolor;
    bool has_circle;
    int  circle_color;
    // Maybe icon/shape once I know how that works?
};

typedef enum
{
    POLICY_RANDOM = 0,
    POLICY_DQPRM = 1
} Policy_e;

class CellTypeInfo
{
public:
    CellInfo data[NUM_CELL_TYPE_INFO] =
        {
            {true,  WHITE,  false, BLACK }, // EMPTY
            {false, BLACK,  false, BLACK }, // WALL
            {false, RED,    false, BLACK }, // RED_BARRIER
            {false, YELLOW, false, BLACK }, // YELLOW_BARRIER
            {false, GREEN,  false, BLACK }, // GREEN_BARRIER
            {true,  WHITE,  true,  RED   }, // RED_BUTTON
            {true,  WHITE,  true,  YELLOW}, // YELLOW_BUTTON
            {true,  WHITE,  true,  GREEN }, // GREEN_BUTTON
            {true,  BLUE,   false, BLACK }  // GOAL
        };

    CellTypeInfo() {}
};

// Forward-declaration of ButtonsEnvironment (needed to allow ButtonsEnvironmentThread to have a reference to the env)
class ButtonsEnvironment;

class ButtonsEnvironmentThread : public ofThread
{
private:
    // Settings like number of ticks, tick rate, etc.
    ofTimer timer;
    uint64_t timer_interval;  // Nanoseconds between ticks, 0 to disable
    int ticks;                // Tick counter
    int tick_limit;           // Stop running after this many ticks
    ButtonsEnvironment &env;  // Reference to the environment

public:
    ButtonsEnvironmentThread(ButtonsEnvironment &_env) :
        timer(),
        timer_interval(0),  // Disable timer by default
        ticks(0),
        tick_limit(10000),
        env(_env)
    {
    }

    void setTickLimit(int _limit)
    {
        tick_limit = _limit;
    }

    void setTimerInterval(uint64_t _interval)
    {
        timer_interval = _interval;
    }

    void threadedFunction();
};

class ButtonsEnvironment {
private:
    // TODO: Replace with thread-safe double-buffered object so we can draw and update in parallel.
    //       The only impact of not doing this is possibly drawing a corrupt environment state from
    //       time to time. No risk of environment corruption.
    CellType_e env_state[10][10] = {};  // Default to all empty cells. Fill in others in constructor.

    CellTypeInfo cell_info;
    ButtonsEnvironmentThread env_thread;
    vector<std::shared_ptr<Agent>> agents;
    ButtonsEnvironmentConfig_e cfg;
    Policy_e policy = POLICY_RANDOM;
    bool goal_reached;

public:
	ButtonsEnvironment();

    void reset(bool _recreate_agents);

    void setPolicy(const Policy_e& policy);
	void setConfig(ButtonsEnvironmentConfig_e _cfg);    // Set up agent config for environment

	void tick();    // Run environment and agents inside it for one cycle
	void render(ofFbo _fbo);  // Renders graphical representation of environment to ofFbo object
	void go(uint64_t _timer_interval, int _tick_limit);
    bool isOpen(ofVec2f _loc);  // Check if agent can move into grid square
	void updateEnvironment();    // Run automatic tasks like lowering barriers when buttons are pressed
	void environmentStep(std::vector<AgentAction_e> &_action, int &_reward, std::vector<Event> &_labels, std::vector<int> &_new_state);   // Duplicates some other code, but need to split for Q-Learning
	void broadcastRandomEvent(); // TODO: Placeholder until I see how to integrate with Rob's code
    bool solved();
    vector<std::shared_ptr<Agent>> &getAgents() { return agents; }
    void getMDPLabel(std::vector<int> &_next_state, MachineState _u, std::vector<Event> &_labels);


private:
    void enableGreenBarrier(bool _enable);
    void enableYellowBarrier(bool _enable);
    void enableRedBarrier(bool _enable);
    void drawAgent(ofFbo _fbo, Agent &_agent);
    void resetAgentPositions();
    ofVec2f yellow_button_loc;
    ofVec2f red_button_loc;
    ofVec2f green_button_loc;
    ofVec2f goal_loc;
    ofVec2f agent1_init_loc;
    ofVec2f agent2_init_loc;
    ofVec2f agent3_init_loc;
    std::shared_ptr<SparseRewardMachine> reward_machine;
    MachineState u;
};
