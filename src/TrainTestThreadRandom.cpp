//
//  TrainTestThreadRandom.cpp
//  Thread for training/testing
//
//

#include <stdlib.h>

#include "TrainTestThreadRandom.hpp"
#include "ButtonsEnvironment.hpp"
#include "soas_marl.h"

void TrainTestThreadRandom::trainTestInit()
{
    printf("trainTestInit\n");
    counter = 0;
    limit=20;
}

bool TrainTestThreadRandom::trainTestLoop()
{
    bool done = false;

    printf("Loop %d\n", counter);

    counter++;
    if( counter >= limit ) done = true;

    return done;
}

void TrainTestThreadRandom::trainTestPost()
{
    printf("trainTestPost\n");
}

