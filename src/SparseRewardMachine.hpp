//
//  SparseRewardMachine.hpp
//  RewardMachines
//
//

#ifndef SparseRewardMachine_hpp
#define SparseRewardMachine_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <map>
#include <sstream>

typedef std::string Event;
typedef int MachineState;
typedef int Reward;
typedef int Unknown;

typedef enum
{
    RM_BUTTONS_1 = 0,
    RM_BUTTONS_2 = 1,
    RM_BUTTONS_3 = 2,
    RM_BUTTONS_TEAM = 3
} SparseRewardMachineFlavor_e;


struct Transition {
    MachineState state_from;
    MachineState state_to;
    Reward reward;
    Event event;
};

class SparseRewardMachine {
private:
    std::set<MachineState> U = {};
    std::unordered_set<Event> events = {};
    MachineState u0;
    std::unordered_map<MachineState, std::unordered_map<Event, MachineState>> delta_u = {};
    std::unordered_map<MachineState, std::unordered_map<MachineState, Reward>> delta_r = {};
    std::unordered_set<MachineState> T = {};
    
protected:
    void add_transition(Transition& transition) {
        this->add_state(transition.state_from);
        this->add_state(transition.state_to);
        this->delta_u[transition.state_from][transition.event] = transition.state_to;
        this->delta_r[transition.state_from][transition.state_to] = transition.reward;
    }
    
    void add_state(MachineState& m) {
        this->U.insert(m);
    }
    
    Reward calculate_reward(const std::vector<Event>& trace) {
        Reward total_reward = 0;
        MachineState current_state = this->get_initial_state();
        
        for (auto& event : trace) {
            MachineState next_state = this->get_next_state(current_state, event);
            Reward reward = this->get_reward(current_state, next_state);
            total_reward += reward;
            current_state = next_state;
        }
        
        return total_reward;
    }
    
    bool is_terminal(const MachineState& u1) {
        for (const auto& u0: this->delta_r) {
            if ((this->delta_r).at(u0.first).find(u1) != (this->delta_r).at(u0.first).end()) {
                return (this->delta_r).at(u0.first).at(u1) == 1;
            }
        }
        return false;
    }
    
public:
    SparseRewardMachine(std::stringstream machine) {
        /*
            0               # initial state
            (0,0,'r1',0)
            (0,1,'r2',0)
            (0,2,'r',0)
            (1,1,'g1',0)
            (1,2,'g2',1)
            (2,2,'True',0)
         */
        this->load(machine);
    }
    
    void load(std::stringstream& machine) {
        machine >> this->u0;
        
        char c;
        MachineState current_state, next_state;
        Event event;
        Reward reward;

        while ((machine >> c >> current_state >> c >> next_state >> c >> event >> c >> reward >> c)) {
            printf("%i -- %i (%s) --> %i\n", current_state, reward, event.c_str(), next_state);
            Transition transition { current_state, next_state, reward, event };
            this->add_transition(transition);
            this->events.insert(transition.event);
        }
        
        for (auto& u1: this->U) {
            if (this->is_terminal_state(u1)) {
                this->T.insert(u1);
            }
        }
    }
    
    MachineState get_initial_state() {
        return this->u0;
    }
    
    MachineState get_next_state(const MachineState& state, const Event& event) {
        if (this->delta_u.find(state) != this->delta_u.end()) {
            if (this->delta_u.at(state).find(event) != this->delta_u.at(state).end()) {
                return this->delta_u.at(state).at(event);
            }
        }
        return state;
    }
    
    Reward get_reward(const MachineState& state1, const MachineState& state2) {
        if (this->delta_r.find(state1) != this->delta_r.end() && this->delta_r.at(state1).find(state2) != this->delta_r.at(state1).end()) {
            return this->delta_r.at(state1).at(state2);
        }
        return 0;
    }
    
    std::pair<std::vector<Reward>, std::vector<MachineState>> get_rewards_and_next_states(const MachineState& s1, const MachineState& s2, const Event& event) {
        std::vector<Reward> rewards {};
        std::vector<MachineState> next_states {};
        for (const MachineState& u1 : this->U) {
            MachineState u2 = this->get_next_state(u1, event);
            rewards.push_back(this->get_reward(u1, u2));
            next_states.push_back(u2);
        }
        return std::make_pair(rewards, next_states);
    }
    
    std::set<MachineState> get_states() {
        return this->U;
    }
    
    bool is_terminal_state(const MachineState& u1) {
        return (this->T.find(u1) != this->T.end());
    }
    
    std::unordered_set<Event> get_events() {
        return this->events;
    }
    
    bool is_event_available(const MachineState& u, const Event& e) {
        return (this->delta_u.find(u) != this->delta_u.end()) &&
        (this->delta_u.at(u).find(e) != this->delta_u.at(u).end());
    }

    static char *getRmBuf(SparseRewardMachineFlavor_e _flavor);
};

#endif /* SparseRewardMachine_hpp */
