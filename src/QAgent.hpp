//
//  QAgent.hpp
//
//

#pragma once

#include <stdio.h>
#include <vector>
#include <string>
#include <unordered_set>
#include <sstream>
#include <random>
#include "SparseRewardMachine.hpp"
#include "Agent.hpp"
#include "Tester.hpp"

// HACK! xtensor calls some variables PI (very bad). Since openFrameworks #defines PI, the variable name
// gets changed to a number, which is nonsense. This is horrible, but there's no time to fix it right, so
// it is what it is.
#ifdef PI
#undef PI
#endif
#include <xtensor/xarray.hpp>
#include <xtensor/xbuilder.hpp>
#include <xtensor/xview.hpp>
#define PI 3.14159265358979323846


class QAgent : public Agent {
private:
    int agent_id;
    std::vector<AgentAction_e> actions {MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT, STAY};
    unsigned int num_states;
    
    SparseRewardMachine rm;
    MachineState u;
    std::unordered_set<Event> local_event_set;
    
    xt::xarray<double, xt::layout_type::dynamic> q;
    Reward total_local_reward = 0;
    bool is_task_complete = false;
    
public:

    QAgent(SparseRewardMachineFlavor_e reward_machine, unsigned int num_states, int agent_id, ButtonsEnvironment &_env, ofVec2f _init_loc) :
        Agent(_env, _init_loc),
        rm (std::stringstream(SparseRewardMachine::getRmBuf(reward_machine))) {
        this->agent_id = agent_id;
            
        std::vector<AgentAction_e> actions = {
            MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT, STAY
        };
            
        this->actions = actions;
        this->num_states = num_states;
        this->u = this->rm.get_initial_state();
        this->local_event_set = this->rm.get_events();
        this->q = xt::zeros<double>({int(num_states), int(this->rm.get_states().size()), int(this->actions.size())});
    }

    
    virtual void initialize_reward_machine() {
        this->u = this->rm.get_initial_state();
        this->is_task_complete = false;
    }

    virtual bool getTaskComplete() { return is_task_complete; }
    
    bool is_local_event_available(const std::vector<Event>& label) {
        if (label.size() > 0) {
            return this->rm.is_event_available(this->u, label[0]);
        }
        else {
            return false;
        }
    }
    
    AgentAction_e get_next_action(const double epsilon, LearningParameters& learning_params)
    {
        double T = learning_params.getT();
        AgentAction_e a_selected = STAY;
        
        if ((double(rand()) / double(RAND_MAX)) < epsilon) {
            // Explore
            auto choice = rand() % this->actions.size();
            a_selected = this->actions[choice];
        }
        else {
            // Exploit
            auto pr_sum = xt::sum(xt::exp(xt::view(this->q, getAgentState(), this->u, xt::all()) * T));
            xt::xarray<double> pr = xt::exp(xt::view(this->q, getAgentState(), this->u, xt::all()) * T) / pr_sum;
            
            // If any q-values are so large that the softmax function returns infinity,
            // make the corresponding actions equally likely
            if (xt::any(xt::isnan(pr))) {
                std::cout << "BOLTZMANN CONSTANT TOO LARGE IN ACTION-SELECTION SOFTMAX." << std::endl;
                xt::xarray<double> temp = xt::xarray<double>(xt::isnan(pr));
                pr = (temp / xt::sum(temp));
            }
            
            xt::xarray<double> pr_select = xt::zeros<double>({int(this->actions.size()) + 1});
            pr_select[0] = 0.0;
            for (size_t i = 0; i < this->actions.size(); i++) {
                pr_select[i+1] = pr_select[i] + pr[i];
            }
            
            double randn = double(rand()) / double(RAND_MAX);
            for (size_t i = 0; i < this->actions.size(); i++) {
                if (randn >= pr_select[i] && randn <= pr_select[i+1]) {
                    a_selected = (AgentAction_e)i;
                    break;
                }
            }
        }
        
        return a_selected;
    }
    
    void update_agent(const MachineState& s_new, const AgentAction_e& a, const Reward& reward, std::vector<Event>& label, LearningParameters& learning_params, bool update_q_function = true) {
        MachineState u_start = this->u;
        
        for (const Event& event : label) {
            MachineState u2 = this->rm.get_next_state(this->u, event);
            this->u = u2;
        }
        
        this->total_local_reward += reward;
        
        if (update_q_function) {
            this->update_q_function(getAgentState(), s_new, u_start, this->u, a, reward, learning_params);
        }
        
        // Note: performAction does this (as well as test validity). May need to combine/change somehow.
        //       env.get_next_state does slip plus validity check
        //       env.environment_step does get_next_state, MDP label, rm: get_next_state and reward
        //this->s = s_new;
        // new_loc.x = s_new%GRID_C;
        // new_loc.y = s_new/GRID_C;
        printf("Performing action %d\n", a);
        this->performAction(a);
        
        if (this->rm.is_terminal_state(this->u)) {
            this->is_task_complete = true;
        }
        
        
    }
    
    void update_q_function(const MachineState& s, const MachineState& s_new, const MachineState& u, const MachineState& u_new, const AgentAction_e& a, const Reward& reward, LearningParameters& learning_params) {
        double alpha = learning_params.getAlpha();
        double gamma = learning_params.getGamma();
        
        double bellman = xt::amax(xt::view(this->q, s_new, u_new, xt::all()))[0];
        this->q(getAgentState(), u, a) = (1.0 - alpha) * this->q(getAgentState(), u, a) + alpha * (reward + gamma * bellman);
    }
    
    virtual void resetAgent(ofVec2f _initial_location) override {
        this->location = _initial_location;
        this->initialize_reward_machine();
    }

    MachineState getCurrentU()
    {
        return u;
    }

    std::set<MachineState> getUSet()
    {
        return rm.get_states();
    }

    bool isUinT(MachineState _u)
    {
        return rm.is_u_in_T(_u);
    }

    SparseRewardMachine &getRM()
    {
        return rm;
    }


};
