//
//  SparseRewardMachine.cpp
//  RewardMachines
//
//

#include "SparseRewardMachine.hpp"

// File-private data
static char Reward_machine_buttons_1[] = "0 (0, 1, by ,0) (1, 2, br ,0) (2, 3, g ,1) (3, 3, True ,0)";
static char Reward_machine_buttons_2[] = "0 (0, 1, by ,0) (1, 2, bg ,0) (2, 3, a2br ,0) (3, 2, a2lr ,0) (3, 4, br ,1) (4, 4, True ,0)";
static char Reward_machine_buttons_3[] = "0 (0, 1, bg ,0) (1, 2, a3br ,0) (2, 1, a3lr ,0) (2, 3, br ,1) (3, 3, True ,0)";
static char Reward_machine_buttons_team[] = "0 (0, 1, by ,0) (1, 2, bg ,0) (2, 3, a2br ,0) (2, 4, a3br ,0) (3, 5, a3br ,0) (3, 2, a2lr ,0) (4, 5, a2br ,0) (4, 2, a3lr ,0) (5, 3, a3lr ,0) (5, 4, a2lr ,0) (5, 6, br ,0) (6, 7, g ,1) (7, 7, True ,0)";
static char *Reward_machine_table[] =
{
    Reward_machine_buttons_1,    // RM_BUTTONS_1
    Reward_machine_buttons_2,    // RM_BUTTONS_2
    Reward_machine_buttons_3,    // RM_BUTTONS_3
    Reward_machine_buttons_team  // RM_BUTTONS_TEAM
};

char *SparseRewardMachine::getRmBuf(SparseRewardMachineFlavor_e _flavor)
{
    char *p_out = NULL;

    switch(_flavor)
    {
        case RM_BUTTONS_1:  // Fall through by design
        case RM_BUTTONS_2:  // Fall through by design
        case RM_BUTTONS_3:  // Fall through by design
        case RM_BUTTONS_TEAM:
            p_out = Reward_machine_table[_flavor];
            break;
        default:
            break;
    }

    return p_out;
}
