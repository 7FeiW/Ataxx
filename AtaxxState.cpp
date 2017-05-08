#include "AtaxxState.h"

AtaxxState::AtaxxState()
{
    size               = 0;
    BlackNum           = 0;
    WhiteNum           = 0;
    numOfJumps         = 0;
    lastMove           = 0;
    numOfPossibleMoves = -1;
    zobristTable       = NULL;
    colorOfNextMove    = UTILITY::COLOR_UNKOWN;
    moveableSpaceNum   = 0;
    numCapOfLastMove   = 0;
}

AtaxxState::~AtaxxState()
{
}

// Copy from input AtaxxState without child information
// This should only be used in create child
void AtaxxState::CopyFromParentNode(const AtaxxState& ataxx)
{
    size     = ataxx.size;
    BlackNum = ataxx.BlackNum;
    WhiteNum = ataxx.WhiteNum;
    moveableSpaceNum = ataxx.moveableSpaceNum;
    numOfJumps      = ataxx.numOfJumps;
    zobristTable    = ataxx.zobristTable;
    zobristHash     = ataxx.zobristHash;
    colorOfNextMove = ataxx.colorOfNextMove;
    colorOfNextMove = GetChildColor();

    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            gameBoard[i][j]    = ataxx.gameBoard[i][j];
        }
    }
}

bool AtaxxState::IsValiedIndex(int i, int j) const
{
    return (i > -1) && (i < size) && (j > -1) && (j < size);
}

void AtaxxState::InitBoardSize(int boardSize)
{
    size = boardSize;
}

// SetUpBoard
void AtaxxState::SetUpBoard(istream& istream)
{
    BlackNum = 0;
    WhiteNum = 0;

    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            istream >> gameBoard[i][j];
            BlackNum += gameBoard[i][j] == UTILITY::BLACK;
            WhiteNum += gameBoard[i][j] == UTILITY::WHITE;
            moveableSpaceNum += gameBoard[i][j] != UTILITY::BLOCK;
        }
    }

    // computer zobristTable on game board setup
    InitZobrist();
}

char AtaxxState::GetChildColor()
{
    return (colorOfNextMove ==
            UTILITY::BLACK) ? UTILITY::WHITE  : UTILITY::BLACK;
}

void AtaxxState::PrintGameBoard() const
{
    cout << "[ Game Board: ]" << endl;

    for (int i = 0; i < size; i++)
    {
        cout << "[ ";
        for (int j = 0; j < size; j++)
        {
            cout << gameBoard[i][j];
        }
        cout << " ]" << endl;
    }
}

// move to child
AtaxxState * AtaxxState::MoveToChild(int originI,
                                     int originJ,
                                     int targetI,
                                     int targetJ)
{
    AtaxxState *moveTo = NULL;

    moveTo = new AtaxxState();
    moveTo->CopyFromParentNode(*this);
    moveTo->MakeMove(originI, originJ, targetI, targetJ);

    return moveTo;
}

// Function to make a move by given i,j
void AtaxxState::MakeMove(int originI, int originJ, int targetI,
                          int targetJ)
{
    int diffX = abs(originI - targetI);
    int diffY = abs(originJ - targetJ);

    // if this is pass move don't do anything
    if ((diffX == 0) && (diffY == 0))
    {
        return;
    }

    bool isMove = ((diffX < 2) && (diffY < 2));
    char color  = gameBoard[originI][originJ];
    gameBoard[targetI][targetJ] = color;

    // if this is a move only thing different compare to jump is
    // we need update couts for each side
    if (isMove)
    {
        if (color == UTILITY::BLACK)
        {
            BlackNum++;
        }
        else
        {
            WhiteNum++;
        }

        // Update Jump Count
        numOfJumps = 0;
    }
    else
    {
        // if this jump action
        // we need set origin to empty
        gameBoard[originI][originJ] = UTILITY::EMPTY;
        // Update zobristHash as jump is move out from here
        UpdateZobristHashOn(originI, originJ, color);

        // Update Jump Count
        numOfJumps++;
    }
    // now move to location
    gameBoard[targetI][targetJ] = color;
    // Update zobristHash as move to new location
    UpdateZobristHashOn(targetI, targetJ, color);
    // Update Move and Jump tables for Move to new location
    UpdateGameBoardOnMove(targetI, targetJ, color);

    // Update last move record
    lastMove = 1000 * originI + 100 * originJ + 10 * targetI + targetJ;
}

void AtaxxState::UpdateGameBoardOnMove(int i,  int  j, char color)
{
    if ((color != UTILITY::WHITE) && (color != UTILITY::BLACK))
    {
        return;
    }

    // check  neighborhood of this square
    for (int  k = 0; k < 8; ++k)
    {
        int neibI = i + UTILITY::OneStepMovesI[k];
        int neibJ = j + UTILITY::OneStepMovesJ[k];

        // cout << neibI << " " << neibJ << endl;

        // if location is reachable and it is not Empty
        // let us set color to current color
        if (IsValiedIndex(neibI, neibJ) &&
            (gameBoard[neibI][neibJ] != color))
        {
            // if neib is black
            // change it to current color
            // reduceb black num, increase white num
            if (gameBoard[neibI][neibJ] == UTILITY::BLACK)
            {
                gameBoard[neibI][neibJ] = color;
                BlackNum--;
                WhiteNum++;
                numCapOfLastMove++;
                UpdateZobristHashOn(neibI, neibJ, UTILITY::BLACK);
                UpdateZobristHashOn(neibI, neibJ, UTILITY::WHITE);

                // cout << BlackNum << " " << WhiteNum << endl;
            }

                // if neib is white
                // change it to current color
                // reduceb black num, increase black num
            else if (gameBoard[neibI][neibJ] == UTILITY::WHITE)
            {
                gameBoard[neibI][neibJ] = color;
                WhiteNum--;
                BlackNum++;
                numCapOfLastMove++;

                UpdateZobristHashOn(neibI, neibJ, UTILITY::BLACK);
                UpdateZobristHashOn(neibI, neibJ, UTILITY::WHITE);

                // cout << BlackNum << " " << WhiteNum << endl;
            }
        }
    }
}

// Function to compute hersitc
int AtaxxState::GetScore() const
{
	int score = BlackNum - WhiteNum;
    if(BlackNum == 0)
	{
		score = - size * size;
	}
	else if(WhiteNum == 0)
	{
		score = size * size;
	}
	return score;
}

int AtaxxState::GetLastMove() const
{
    return lastMove;
}

string AtaxxState::GetLastMoveStr() const
{
    return DecodeLastMove(lastMove);
}

int AtaxxState::GetIndexByColor(char color)
{
    return (color == UTILITY::BLACK) ? 0 : 1;
}

void AtaxxState::GenerateMoves()
{
    if(colorOfNextMove == UTILITY::COLOR_UNKOWN)
    {
        return;
    }

    int moveIdx = 0;
    int td = (colorOfNextMove == UTILITY::BLACK) ? BlackNum : WhiteNum;
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            if(moveableSpaceNum < 2*td)
            {
                // if we can move to here
                if (gameBoard[i][j] == UTILITY::EMPTY)
                {
                    // can I move to here?
                    // figure out which squre can move to this location
                    for (int k = 0; k < 8; ++k)
                    {
                        int neibI = i + UTILITY::OneStepMovesI[k];
                        int neibJ = j + UTILITY::OneStepMovesJ[k];

                        if (IsValiedIndex(neibI, neibJ) &&
                            (gameBoard[neibI][neibJ] == colorOfNextMove))
                        {
                            possibleMoves[moveIdx] = GetEncodeMove(neibI,neibJ,i,j);
                            moveIdx++;
                            break;
                        }
                    }
                    // lets  jump
                    // figure out which square can jump to this location
                    for (int k = 0; k < 16; ++k)
                    {
                        int neibI = i + UTILITY::TwoStepMovesI[k];
                        int neibJ = j + UTILITY::TwoStepMovesJ[k];

                        if (IsValiedIndex(neibI, neibJ) &&
                            (gameBoard[neibI][neibJ] == colorOfNextMove))
                        {
                            possibleMoves[moveIdx] = GetEncodeMove(neibI,neibJ,i,j);

                            moveIdx++;
                        }
                    }
                }
            }
            else
            {
                // if we can move from here
                if (gameBoard[i][j] == colorOfNextMove)

                    // figure out which location we can move into
                    for (int k = 0; k < 8; ++k)
                    {
                        int neibI = i + UTILITY::OneStepMovesI[k];
                        int neibJ = j + UTILITY::OneStepMovesJ[k];

                        if (IsValiedIndex(neibI, neibJ) &&
                            (gameBoard[neibI][neibJ] == UTILITY::EMPTY))
                        {
                            possibleMoves[moveIdx] = GetEncodeMove(i,j,neibI,neibJ);
                            moveIdx++;
                        }
                    }
                    // lets  jump
                    // figure out which square can jump to this location
                    for (int k = 0; k < 16; ++k)
                    {
                        int neibI = i + UTILITY::TwoStepMovesI[k];
                        int neibJ = j + UTILITY::TwoStepMovesJ[k];

                        if (IsValiedIndex(neibI, neibJ) &&
                            (gameBoard[neibI][neibJ] == UTILITY::EMPTY))
                        {
                            possibleMoves[moveIdx] = GetEncodeMove(i,j,neibI,neibJ);
                            moveIdx++;
                        }
                    }
                }
            }
    }
    numOfPossibleMoves = moveIdx;
}

void AtaxxState::PrintInfo() const
{
    cout << "Last Move: " << GetLastMove() << endl;
    cout << "Score: " << GetScore() << endl;
    cout << "Black Num: " << BlackNum << endl;
    cout << "White Num: " << WhiteNum << endl;
    cout << "Child Num: " << numOfPossibleMoves << endl;
    cout << "Hash: " << zobristHash << endl;
    PrintGameBoard();
}

void AtaxxState::SetZoristTable(unsigned long long ***zobrist)
{
    zobristTable = zobrist;
}

void AtaxxState::InitZobrist()
{
    zobristHash = 0;

    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            if (gameBoard[i][j] == UTILITY::BLACK)
            {
                zobristHash ^= zobristTable[i][j][0];
            }
            else if (gameBoard[i][j] == UTILITY::WHITE)
            {
                zobristHash ^= zobristTable[i][j][1];
            }
        }
    }
}

void AtaxxState::UpdateZobristHashOn(int i, int j, char color)
{
    int idx = GetIndexByColor(color);

    if (zobristTable != NULL)
    {
        zobristHash ^= zobristTable[i][j][idx];
    }
}

bool AtaxxState::HasNoMoreMove()
{
    if (numOfPossibleMoves == -1)
    {
        GenerateMoves();
    }
    return numOfPossibleMoves == 0;
}

bool AtaxxState::ExceededJumpLimit() const
{
    return numOfJumps > maxJumpAllowed;
}

bool AtaxxState::BothSideHadNoMoreMove()
{
    // First let us check if current color has any moves
    return HasNoMoreMove();
}
void AtaxxState::SetColorForNextMove(char color)
{
    colorOfNextMove = color;
}

char AtaxxState::GetColorForNextMove()
{
    return colorOfNextMove;
}

int AtaxxState::GetColorInt(char color)
{
    // Because color saved in this node is for next state
    // ex. if black player ready to move means current state is white
    return (color == UTILITY::BLACK) ? 1 : -1;
}

int AtaxxState::GetEncodeMove(int originI, int originJ, int targetI, int targetJ)
{
    return 1000 * originI + 100 * originJ + 10 * targetI + targetJ;
}

void AtaxxState::MakeMoveWithEncodedLastMove(int lastMove)
{
    int originI = lastMove / 1000;
    int originJ = (lastMove % 1000) / 100;
    int targetI = (lastMove % 100) / 10;
    int targetJ = (lastMove % 10);

    return MakeMove(originI, originJ, targetI, targetJ);
}

string AtaxxState::DecodeLastMove(int lastMove) const
{
    int row     = size - lastMove / 1000 - 1;
    int col     = (lastMove % 1000) / 100;
    int new_row = size - (lastMove % 100) / 10 - 1;
    int new_col = (lastMove % 10);
    // cout << "[Decode " << lastMove << " to " << col << " "<< row << " " << new_col << " " << new_row <<" ]" << endl;
    // for debug
    string decode;

    decode += char('a' + col);
    decode += char('1' + row);
    decode += char('a' + new_col);
    decode += char('1' + new_row);
    return decode;
}

string AtaxxState::GetEndGameString() const
{
    string endGame = "";
    endGame += "[ " + UTILITY::BLACK_OUTPUT + " " +  to_string(BlackNum)
               + " - " + UTILITY::WHITE_OUTPUT + " " +  to_string(WhiteNum)
               + " = " + to_string(BlackNum - WhiteNum) + " ]";
    return endGame;
}

unsigned long long AtaxxState::GetHash() const
{
    return zobristHash;
}

int AtaxxState::AlphaBetaSearch(AtaxxState             *state,
                                    int                     depth,
                                    int                     alpha,
                                    int                     beta,
                                    int                     color,
                                    unordered_map<unsigned long long,
                                            TTEntry>& transTable,
                                    unordered_map<unsigned long long, int>
                                    &stateVisited,
                                    clock_t               & statTime,
                                    const int             & timeAllowed,
                                    int & numAbCalls,
                                    int & numTTQueries,
                                    int & numBetaCut)
{
    //telemetry
    numAbCalls++;
    double elapsedSecs = double((clock() - statTime) / CLOCKS_PER_SEC);

    if (elapsedSecs > timeAllowed)
    {
        throw elapsedSecs;
    }

    // do this up front
    // if next player can not move this is an end game state
    // state->GenerateMoves();

    bool isEndGame = state->HasNoMoreMove() || state->ExceededJumpLimit();
    if(stateVisited.find(state->zobristHash) != stateVisited.end())
    {
        isEndGame |= (stateVisited[state->zobristHash] == 3);
    }
    if ((depth == 0) ||  isEndGame)
    {
        return color * state->GetScore();
    }

    // Before we do anything else
    // looking at TT this will save some time
    int bestMove = 0;

    if (transTable.find(state->zobristHash) != transTable.end())
    {
        // if V ≤ α, then V is an upper bound on the true node value
        // if α < V < β, then V is the true node value
        // if β ≤ V, then V is lower bound on the true node value
        if (transTable[state->zobristHash].Height == depth)
        {
            //telemetry
            numTTQueries++;

            // cout << "transTable" << endl;
            // bestMove = transTable[zobristHash].BestMove;
            int type = transTable[state->zobristHash].Type;

            if (type == transTable[state->zobristHash].LOWER)
            {
                alpha = max(alpha, transTable[state->zobristHash].Score);
            }
            else if (type == transTable[state->zobristHash].UPPER)
            {
                beta = min(beta, transTable[state->zobristHash].Score);
            }
            else
            {
                alpha = beta = transTable[state->zobristHash].Score;
            }

            if (alpha >= beta)
            {
                // TT cut off
                return transTable[state->zobristHash].Score; // TT entry causes
            }

            // More move ordering
            bestMove = transTable[state->zobristHash].BestMove;
        }
    }

    // Move ordering
    int bestValue = -numeric_limits<int>::max();
    // keep track state by hash
    // set<unsigned long long> seenBefore;
    // if we have found best Move in TT
    if (bestMove != 0)
    {
        AtaxxState *bestChild = new AtaxxState();
        bestChild->CopyFromParentNode(*state);
        bestChild->MakeMoveWithEncodedLastMove(bestMove);
        try{
            bestValue = -AlphaBetaSearch(bestChild,
                                         depth - 1,
                                         -beta,
                                         -alpha,
                                         -color,
                                         transTable,
                                         stateVisited,
                                         statTime,
                                         timeAllowed,
                                         numAbCalls,numTTQueries, numBetaCut);
            delete bestChild;
            bestChild = NULL;
            alpha = max(alpha, bestValue);
        }
        catch (double& timeCost)
        {
            delete bestChild;
            bestChild = NULL;
            throw timeCost;
        }
        //seenBefore.insert(bestChild->zobristHash);
    }

    // we don't have a beta cut off from best move
    if (alpha < beta)
    {
        for (int i = 0; i < state->numOfPossibleMoves; ++i)
        {
            if(state->possibleMoves[i] == bestMove)
            {
                continue;
            }
            AtaxxState *child = new AtaxxState();
            child->CopyFromParentNode(*state);
            child->MakeMoveWithEncodedLastMove(state->possibleMoves[i]);
            int value;
            try{
                value = -AlphaBetaSearch(child,
                                         depth - 1,
                                         -beta,
                                         -alpha,
                                         -color,
                                         transTable,
                                         stateVisited,
                                         statTime,
                                         timeAllowed,
                                         numAbCalls,numTTQueries, numBetaCut);
                //seenBefore.insert(child->zobristHash);
                delete child;
            }
            catch (double& timeCost)
            {
                delete child;
                child = NULL;
                throw timeCost;
            }


            if (bestValue < value)
            {
                bestMove  = state->possibleMoves[i];
                bestValue = value;
            }
            alpha = max(alpha, value);

            // beta cut off
            if (alpha >= beta)
            {
                numBetaCut++;
                break;
            }
        }
    }
    else
    {
        numBetaCut++;
    }

    // TT Save
    TTEntry tt;
    tt.Save(bestValue, depth, alpha, beta, bestMove);
    transTable[state->zobristHash] = tt;

    return bestValue;
}

AtaxxState *AtaxxState::FindBestMove(int  depth,
                                           unordered_map<unsigned long long,
                                           TTEntry>& transTable,
                                           unordered_map<unsigned long long, int>
                                           &stateVisited,
                                           clock_t               & statTime,
                                           int                     timeAllowed,
                                           int  mode)
{
    AtaxxState *nextState = new AtaxxState();
    nextState->CopyFromParentNode(*this);
    //Oops End Game, return a pass move
    /*if(nextState->HasNoMoreMove())
    {
        return nextState;
    }*/
    char color = colorOfNextMove;
    int colorInt = GetColorInt(color);

    // iterate deeping
    int bestMove = 0;
    // -1 means time limited only
    if(depth == -1)
    {
        depth = 20;
    }
    // -1 means depth only
    if(timeAllowed == -1)
    {
        timeAllowed = 1000;
    }

    for (int i = 1; i <= depth; ++i)
    {
        int alpha = -numeric_limits<int>::max();
        int beta  = numeric_limits<int>::max();
        int searchReturn = 0;
        int numAbCalls = 0;
        int numTTQueries = 0;
        int numBetaCut = 0;
        // int childIdx = NegaMaxSearch(this, i, colorInt, 0);
        try {
            if(mode == search_mode_simple)
            {
                if(transTable.size() >= 256000)
                {
                    transTable.clear();
                }
                searchReturn = AlphaBetaSearch(this,
                                               i,
                                               alpha,
                                               beta,
                                               colorInt,
                                               transTable,
                                               stateVisited,
                                               statTime,
                                               timeAllowed, numAbCalls,numTTQueries, numBetaCut);
            }
            else if(mode ==  search_mode_advanced)
            {
                searchReturn = NegaScoutSearch(this,
                                               i,
                                               alpha,
                                               beta,
                                               colorInt,
                                               transTable,
                                               stateVisited,
                                               statTime,
                                               timeAllowed, numAbCalls,numTTQueries, numBetaCut);

            }
            bestMove = transTable[zobristHash].BestMove;
        }
        catch (double& timeCost)
        {
            // Time Up
            cout << "[Time used For this search is " << timeCost << "s ]" << endl;

            // throw nextState back
            nextState->MakeMoveWithEncodedLastMove(bestMove);
            throw nextState;

            // Doing nothing for now
        }
        //Search out put
        if(color == UTILITY::BLACK)
        {
            cout << UTILITY::BLACK_OUTPUT << " ";
        }
        else
        {
            cout << UTILITY::WHITE_OUTPUT << " ";
        }

        double timeEscap = double(clock() - statTime) / CLOCKS_PER_SEC;
        // print depth and Time
        cout << " " << setw(3) <<  i << " " << right << setw(5) << fixed << setprecision(2) << timeEscap;
        cout << " [ ";
        cout << right  << setw(10) << numAbCalls << " ";
        cout << right  << setw(10) << setprecision(2) << (double)numAbCalls/timeEscap << " ";
        cout << right  << setw(10) << numTTQueries << " ";
        cout << right  << setw(10) << transTable.size() << " ";
        cout << right  << setw(10) << numBetaCut << " ";
        cout << "]";
        cout << "[";
        cout << right  << setw(4) << DecodeLastMove(bestMove) << " ";
        AtaxxState second = AtaxxState();
        second.CopyFromParentNode(*this);
        second.MakeMoveWithEncodedLastMove(bestMove);
        int nextBestMove = 0;
        if(i > 1)
        {
            nextBestMove = transTable[second.zobristHash].BestMove;
            cout << right  << setw(4) <<  DecodeLastMove(nextBestMove) << " ";
        }
        else
        {
            cout << "---- ";
        }
        if(i > 2)
        {
            AtaxxState third = AtaxxState();
            third.CopyFromParentNode(second);
            third.MakeMoveWithEncodedLastMove(nextBestMove);
            nextBestMove = transTable[third.zobristHash].BestMove;
            cout << right  << setw(4) <<  DecodeLastMove(nextBestMove) << " ";
        }
        else
        {
            cout << "---- ";
        }
        cout << "]";
        cout << " " << showpos << searchReturn << noshowpos << endl;
        cout << flush;
    }
    nextState->MakeMoveWithEncodedLastMove(bestMove);

    // remove all other child
    // we need save memory space
    return nextState;
}


int AtaxxState::NegaScoutSearch(AtaxxState             *state,
                                    int                     depth,
                                    int                     alpha,
                                    int                     beta,
                                    int                     color,
                                    unordered_map<unsigned long long,
                                            TTEntry>& transTable,
                                    unordered_map<unsigned long long, int>&stateVisited,
                                    clock_t               & statTime,
                                    const int             & timeAllowed,
                                    int & numAbCalls,
                                    int & numTTQueries,
                                    int & numBetaCut)
{
    //telemetry
    numAbCalls++;
    double elapsedSecs = double(clock() - statTime) / CLOCKS_PER_SEC;

    if (elapsedSecs > timeAllowed)
    {
        throw elapsedSecs;
    }

    // do this up front
    // if next player can not move this is an end game state
    // state->GenerateMoves();

    // First we need check if we have an end game or at the bottom of the sea
    bool isEndGame = state->HasNoMoreMove() || state->ExceededJumpLimit();
    if(stateVisited.find(state->zobristHash) != stateVisited.end())
    {
        isEndGame |= (stateVisited[state->zobristHash] == 3);
    }
    if ((depth <= 0) ||  isEndGame)
    {
        return color * state->GetScore();
    }

    // Before we do anything else
    // looking at TT this will save some time

    if (transTable.find(state->zobristHash) != transTable.end())
    {
        // if V ≤ α, then V is an upper bound on the true node value
        // if α < V < β, then V is the true node value
        // if β ≤ V, then V is lower bound on the true node value
        if (transTable[state->zobristHash].Height == depth)
        {
            //telemetry
            numTTQueries++;

            // cout << "transTable" << endl;
            // bestMove = transTable[zobristHash].BestMove;
            int type = transTable[state->zobristHash].Type;

            if (type == transTable[state->zobristHash].LOWER)
            {
                alpha = max(alpha, transTable[state->zobristHash].Score);
            }
            else if (type == transTable[state->zobristHash].UPPER)
            {
                beta = min(beta, transTable[state->zobristHash].Score);
            }
            else
            {
                alpha = beta = transTable[state->zobristHash].Score;
            }

            if (alpha >= beta)
            {
                // TT cut off
                return transTable[state->zobristHash].Score; // TT entry causes
            }
        }
    }

    array <AtaxxState* , state->maxPossibleMoves> childs;
    // keep track state by hash
    //set<unsigned long long> seenBefore;
    for(int i = 0 ; i < state->maxPossibleMoves; ++i)
    {
        if(i < state->numOfPossibleMoves)
        {
            int moveInt = state->possibleMoves[i];
            AtaxxState *c = new AtaxxState();
            c->CopyFromParentNode(*state);
            c->MakeMoveWithEncodedLastMove(moveInt);
            childs[i] = c;
        }
        else
        {
            childs[i] = NULL;
        }
    }

    sort(childs.begin(),childs.begin()+state->numOfPossibleMoves,
         [](AtaxxState* a, AtaxxState * b)
         {
             return a->numCapOfLastMove < b->numCapOfLastMove;
         });

    int score;
    try
    {
        score = -NegaScoutSearch(childs[0],
                                 depth - 1,
                                 -beta,
                                 -alpha,
                                 -color,
                                 transTable,
                                 stateVisited,
                                 statTime,
                                 timeAllowed,
                                 numAbCalls,numTTQueries, numBetaCut);
    }
    catch (double& timeCost)
    {
        for(int i = 0 ; i < maxPossibleMoves; ++i)
        {
            if(childs[i] != NULL)
            {
                delete childs[i];
            }
        }
        throw timeCost;
    }


    int bestMove = childs[0]->lastMove;
    //.insert(childs[0]->zobristHash);
    if(score < beta)
    {
        int lBound = max(score, alpha);
        int uBound = lBound + 1;
        // other children: 0-window search + re-search if needed
        for (int i = 1; i < state->numOfPossibleMoves; ++i)
        {
            int result;
            try {
                result = -NegaScoutSearch(childs[i],
                                          depth - 1,
                                          -uBound,
                                          -lBound,
                                          -color,
                                          transTable, stateVisited,
                                          statTime,
                                          timeAllowed,
                                          numAbCalls, numTTQueries, numBetaCut);
            }
            catch (double& timeCost)
            {
                for(int i = 0 ; i < maxPossibleMoves; ++i)
                {
                    if(childs[i] != NULL)
                    {
                        delete childs[i];
                    }
                }
                throw timeCost;
            }
            //seenBefore.insert(childs[i]->zobristHash);
            if (result >= uBound && result < beta)
            {
                try {
                    result = -NegaScoutSearch(childs[i],
                                              depth - 1,
                                              -beta,
                                              -result,
                                              -color,
                                              transTable,
                                              stateVisited,
                                              statTime,
                                              timeAllowed,
                                              numAbCalls, numTTQueries, numBetaCut);
                }
                catch (double& timeCost)
                {
                    for(int i = 0 ; i < maxPossibleMoves; ++i)
                    {
                        if(childs[i] != NULL)
                        {
                            delete childs[i];
                        }
                    }
                    throw timeCost;
                }
            }

            if (result > score)
            {
                score = result;
                bestMove = childs[i]->lastMove;
            }
            if (result >= beta)
            {
                numBetaCut +=  state->numOfPossibleMoves - i +1;
                break;
            }

        }

    }

    for(int i = 0 ; i < maxPossibleMoves; ++i)
    {
        if(childs[i] != NULL)
        {
            delete childs[i];
        }
    }

    // TT Save
    if(!bestMove == 0)
    {
        TTEntry tt;
        tt.Save(score, depth, alpha, beta, bestMove);
        transTable[state->zobristHash] = tt;
    }
    return score;
}