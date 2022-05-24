//
//  QAgent.hpp
//
//

#ifndef QAgent_hpp
#define QAgent_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include <unordered_set>
#include <sstream>
#include "SparseRewardMachine.hpp"
#include <xtensor/xarray.hpp>
#include <xtensor/xbuilder.hpp>
#include <xtensor/xview.hpp>
#include <random>


typedef int Action;

struct LearningParams {
    double T;
    double alpha;
    double gamma;
};

class QAgent {
private:
    int agent_id;
    int s_i; // index of initial state
    MachineState s;
    std::vector<Action> actions;
    unsigned int num_states;
    
    SparseRewardMachine rm;
    MachineState u;
    std::unordered_set<Event> local_event_set;
    
    xt::xarray<double, xt::layout_type::dynamic> q;
    Reward total_local_reward = 0;
    bool is_task_complete = false;
    
public:
    
    QAgent(std::stringstream& reward_machine, MachineState s_i, unsigned int num_states, std::vector<Action> actions, int agent_id) : rm {SparseRewardMachine(reward_machine)} {
        this->agent_id = agent_id;
        this->s_i = s_i;
        this->s = s_i;
        this->actions = actions;
        this->num_states = num_states;
        
        this->u = this->rm.get_initial_state();
        this->local_event_set = this->rm.get_events();
        
        this->q = xt::zeros<double>({int(num_states), int(this->rm.get_states().size()), int(this->actions.size())});
    }
    
    void reset_state() {
        this->s = this->s_i;
    }
    
    void initialize_reward_machine() {
        this->u = this->rm.get_initial_state();
        this->is_task_complete = false;
    }
    
    bool is_local_event_available(const std::vector<Event>& label) {
        if (label.size() > 0) {
            return this->rm.is_event_available(this->u, label[0]);
        }
        else {
            return false;
        }
    }
    
    std::pair<MachineState, Action> get_next_action(const double epsilon, const LearningParams& learning_params) {
        auto T = learning_params.T;
        Action a_selected = NULL;
        
        if ((double(rand()) / double(RAND_MAX)) < epsilon) {
            // Explore
            auto choice = rand() % this->actions.size();
            a_selected = this->actions[choice];
        }
        else {
            // Exploit
            auto pr_sum = xt::sum(xt::exp(xt::view(this->q, this->s, this->u, xt::all()) * T));
            xt::xarray<double> pr = xt::exp(xt::view(this->q, this->s, this->u, xt::all()) * T) / pr_sum;
            
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
            for (const Action& a : this->actions) {
                if (randn >= pr_select[a] && randn <= pr_select[a+1]) {
                    a_selected = a;
                    break;
                }
            }
        }
        
        return std::make_pair(this->s, a_selected);
    }
    
    void update_agent(const MachineState& s_new, const Action& a, const Reward& reward, const std::vector<Event>& label, const LearningParams& learning_params, bool update_q_function = true) {
        MachineState u_start = this->u;
        
        for (const Event& event : label) {
            MachineState u2 = this->rm.get_next_state(this->u, event);
            this->u = u2;
        }
        
        this->total_local_reward += reward;
        
        if (update_q_function) {
            this->update_q_function(this->s, s_new, u_start, this->u, a, reward, learning_params);
        }
        
        this->s = s_new;
        
        if (this->rm.is_terminal_state(this->u)) {
            this->is_task_complete = true;
        }
    }
    
    void update_q_function(const MachineState& s, const MachineState& s_new, const MachineState& u, const MachineState& u_new, const Action& a, const Reward& reward, const LearningParams& learning_params) {
        double alpha = learning_params.alpha;
        double gamma = learning_params.gamma;
        
        double bellman = xt::amax(xt::view(this->q, s_new, u_new, xt::all()))[0];
        this->q(s, u, a) = (1.0 - alpha) * this->q(s, u, a) + alpha * (reward + gamma * bellman);
    }
};

#endif /* QAgent_hpp */
