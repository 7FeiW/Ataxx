//

// Created by tellf on 2017/2/7.
//
#include "TTEntry.h"

void TTEntry::Save(int score,
                   int height, int alpha, int beta, int move)
{
    this->Score  = score;
    this->Height = height;

    if (score <= alpha)
    {
        this->Type = this->UPPER; // value is upper bound
    }
    else if (score >= beta)
    {
        this->Type =  this->LOWER; // value is lower bound
    }
    else
    {
        this->Type =  this->ACCURATE;
    }
    this->BestMove = move;
}
