#pragma once

// Include file with common stuff for SOAS-MARL project
#define FRAME_RATE 60

// This section deals with environment size in pixels
#define SQUARE_LEN 25
#define BUTTON_RADIUS 10
#define GRID_R 10
#define GRID_C 10
// Margin also set to same size as square but doesn't have to be
#define MARGIN 25

#define ENV_WIDTH (GRID_C*SQUARE_LEN)
#define ENV_HEIGHT (GRID_R*SQUARE_LEN)

#define ENV_WIDTH_PLUS_MARGIN ( (2*MARGIN) + ENV_WIDTH )
#define ENV_HEIGHT_PLUS_MARGIN ( (2*MARGIN) + ENV_HEIGHT )

// This section deals with window setup parameters
// Used to set up space in window for grid of environments
// We'll start with 1x1
#define NUM_ENV_ROW 2
#define NUM_ENV_COL 2

//#define SLIP_PROBABILITY 0.02
#define SLIP_PROBABILITY 0

#define WINDOW_WIDTH (NUM_ENV_COL*ENV_WIDTH_PLUS_MARGIN)
#define WINDOW_HEIGHT (NUM_ENV_ROW*ENV_HEIGHT_PLUS_MARGIN)

