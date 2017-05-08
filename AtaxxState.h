#pragma once

#include <cmath>
#include <iostream>
#include <iomanip>
#include <string>
#include <ctime>
#include <cstdlib>
#include <limits>
#include <random>
#include <unordered_map>
#include <set>
#include <array>
#include <algorithm>
#include "TTEntry.h"
#include "Utility.h"
using namespace std;

class AtaxxState {
protected:
    // Total allowed jumps
    static const int maxJumpAllowed   = 50;
    // max number of possible moves
    static const int maxPossibleMoves = 1000;

    static const int search_mode_advanced = 2;
    static const int search_mode_simple = 1;
    // Game board
    // where e is Empty, x is block, w is White, and b is black
    char gameBoard[8][8];

    // zobristTable
    unsigned long long ***zobristTable;
    unsigned long long zobristHash;

    // maxBoardSize of game board
    int size;

    // number of black
    int BlackNum;

    // number of White
    int WhiteNum;

    //num of moveabnle space
    int moveableSpaceNum;

    // num of Jumps has been performed
    int numOfJumps;

    // int encode for movenet
    int lastMove;

    // int number of caps
    int numCapOfLastMove;

    // color of next move:
    char colorOfNextMove;

    // Num Of moves for current player
    int numOfPossibleMoves;

    // Num Of moves for current player
    int possibleMoves[maxPossibleMoves];

    // function to get idx by color
    int  GetIndexByColor(char color);

    // function to get -1 and 1 for search
    int  GetColorInt(char color);

    // function to get child color
    char GetChildColor();

    // function to check if given i and j are within board limited
    bool IsValiedIndex(int i,
                       int j) const;

    // Move stone from origin to taregt
    void MakeMove(int originI,
                  int originJ,
                  int targetI,
                  int targetJ);


    // this function handle cap in ataxx
    void UpdateGameBoardOnMove(
        int  i,
        int  j,
        char color);

    // move is encode
    int GetEncodeMove(int originI,
                      int originJ,
                      int targetI,
                      int targetJ);

    // Make move with encode int
    void MakeMoveWithEncodedLastMove(int lastMove);
    // function to decode move int to string
    string DecodeLastMove(int lastMove) const;
    // function get last move
    int GetLastMove() const;
    // function get last Score
    int  GetScore() const;
    // copy from parent node , and set current color based on
    // parent node
    void CopyFromParentNode(const AtaxxState& ataxx);
    // function to create all possible moves
    void GenerateMoves();
    // function to Update Zobrist HashOn i and j with color
    void UpdateZobristHashOn(int  i,
                             int  j,
                             char color);
public:

    AtaxxState();
    virtual ~AtaxxState();
    // debug functions
    void        PrintInfo() const;

    // function to init game
    void        InitBoardSize(int boardSize);
    // function to setup game board
    void        SetUpBoard(istream& istream);
    // function to print game board
    void        PrintGameBoard() const;
    // function to handle move and return a point to new state
    AtaxxState* MoveToChild(int originI,
                            int originJ,
                            int targetI,
                            int targetJ);

    // function to return hash
    unsigned long long  GetHash() const;
    // function to get last Move as String hash
    string GetLastMoveStr() const;
    // function to init zobrits
    void InitZobrist();
    // function to set zorist hash table
    // shoud only be used in the init of game
    void SetZoristTable(unsigned long long ***zobrist);

    void SetColorForNextMove(char color);
    char GetColorForNextMove();
    // Function to tell if current state is end game;
    bool HasNoMoreMove();
    // function to tell if exceeded Jump limitation
    bool ExceededJumpLimit() const;
    // Function to tell if both side has no more move
    bool BothSideHadNoMoreMove();
    // Function to Return End Game String
    string GetEndGameString()const;


    int AlphaBetaSearch(AtaxxState *state,
                        int depth,
                        int alpha,
                        int beta,
                        int color,
                        unordered_map<unsigned long long, TTEntry>& transTable,
                        unordered_map<unsigned long long, int>&stateVisited,
                        clock_t& statTime,
                        const int& timeAllowed,
                        int & numAbCalls,
                        int & numTTQueries,
                        int & numBetaCut);
    // NegaScoutSearch
    int NegaScoutSearch(AtaxxState *state,
                        int depth,
                        int alpha,
                        int beta,
                        int color,
                        unordered_map<unsigned long long, TTEntry>& transTable,
                        unordered_map<unsigned long long, int>&stateVisited,
                        clock_t& statTime,
                        const int& timeAllowed,
                        int & numAbCalls,
                        int & numTTQueries,
                        int & numBetaCut);

    // function to find best  move  for current playerand return a point to new state
    virtual AtaxxState* FindBestMove(int depth,
                                         unordered_map<unsigned long long, TTEntry>& transTable,
                                         unordered_map<unsigned long long, int>
                                         &stateVisited,
                                         clock_t & statTime,
                                         int timeAllowed = numeric_limits<int>::max(),
                                         int mode = 1);

};

