//
//  ButtonsEnvironment.hpp
//  Environment for the buttons team task
//
#pragma once

#include <stdio.h>  // For debugging

#include "ofMain.h"

#define WHITE  0xffffff
#define BLACK  0x000000
#define RED    0xff0000
#define GREEN  0x00ff00
#define BLUE   0x0000ff
#define YELLOW 0xffff00

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

class ButtonsEnvironment {
private:
    // TODO: Replace with thread-safe double-buffered object so we can draw and update in parallel.
    CellType_e env_state[10][10] = {};  // Default to all empty cells. Fill in others in constructor.

    CellTypeInfo cell_info;

public:
	ButtonsEnvironment();

	void tick();    // Run environment and agents inside it for one cycle
	void render(ofFbo _fbo);  // Renders graphical representation of environment to ofFbo object

private:
    void enableGreenBarrier(bool _enable);
    void enableYellowBarrier(bool _enable);
    void enableRedBarrier(bool _enable);
};
