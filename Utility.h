//

// Created by tellf on 2017/2/7.
//

#pragma once
#define UTILITY
#include <ctime>

// Definition of board colors
static const char EMPTY        = 'e';
static const char BLACK        = 'b';
static const char WHITE        = 'w';
static const char BLOCK        = 'x';
static const char COLOR_UNKOWN = 'u';

// Definition of directions
static const int OneStepMovesI[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
static const int OneStepMovesJ[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
static const int TwoStepMovesI[16] = { -2, -2, -2, -2, -2, -1, -1, 0, 0, 1, 1, 2, 2, 2, 2, 2 };
static const int TwoStepMovesJ[16] = { -2, -1,  0,  1,  2, -2,  2, -2, 2, -2, 2, -2, -1, 0, 1, 2 };

// Output strings
static const std::string BLACK_OUTPUT = "##";
static const std::string WHITE_OUTPUT = "()";
