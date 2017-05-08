#pragma once

#include "AtaxxState.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>
#include "TTEntry.h"
#include "Utility.h"
#include <stack>
#include <ctime>
#include "AtaxxState.h"

using namespace std;

class Ataxx {
private:

    // current gameState
    AtaxxState *gameState;

    // stack of game state, so that we can do undo
    stack<AtaxxState *>gameHistory;

    // trans table
    unordered_map<unsigned long long, TTEntry>transTable;

    // state cout
    unordered_map<unsigned long long, int>stateVisited;

    // searach setting
    int searchSetting;

    // max search depth
    int maxSearchDepth;

    // game board size
    int size;

    // this color
    char color;

    // GameEnd
    bool gameEnd;

    // search time limit in second
    int fixSearchTime;
    double remainSearchTime;

    // zobristTable
    unsigned long long ***zobrist;

    const int maxBoardSize = 8;
    const int numColor     = 2;

    // Function check if move is legal
    bool IsLegalLocation(int i);

    // Function to InitZobrist in the begin of the game;
    void InitZobrist();

    // Function to Clean all history
    // used in init and destructor
    void CleanHistroy();

    // Function to Clean Zobrist Table
    void CleanZobristTable();

    // Function to Update End Game Flag
    void UpdateEndGameFlag();

public:

    const int SearchSetting_UNKNOWN   = 0;
    const int SearchSetting_Fixed     = 1;
    const int SearchSettings_Advanced = 2;
    const int MaxAllowedRepeatState   = 3;

    Ataxx();
    ~Ataxx();
    void Init(int n);
    void SetCurrentPlayer(char color);
    void MoveCurrentPlayer(int col,
                           int row,
                           int new_col,
                           int new_row);
    void   SetUpBoard(istream& istream);
    void   PrintGameBoard();
    void   SetSearchSetting(const int setting);
    void   SetSearchDepth(const int depth);
    void   Undo();
    void   Debug();
    void   SetFixSearchTime(const int time);
    void   SetRemainSearchTime(const int time);
    string FindBestMove();

};
