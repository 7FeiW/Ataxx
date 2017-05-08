#include "Ataxx.h"

Ataxx::Ataxx()
{
    this->gameState        = NULL;
    this->searchSetting    = this->SearchSetting_UNKNOWN;
    this->maxSearchDepth   = -1;
    this->color            = UTILITY::COLOR_UNKOWN;
    this->zobrist          = NULL;
    this->fixSearchTime    = -1;
    this->remainSearchTime = -1;
    this->InitZobrist();
}

Ataxx::~Ataxx() {
    this->CleanZobristTable();
    this->CleanHistroy();

    if (gameState != NULL)
    {
        delete gameState;
    }
    gameState = NULL;
}

void Ataxx::Init(int n)
{
    if (this->gameState != NULL)
    {
        delete this->gameState;
    }
    this->InitZobrist();
    this->gameState = new AtaxxState();
    this->gameState->SetZoristTable(this->zobrist);
    this->gameState->InitBoardSize(n);
    this->size = n;
    this->CleanHistroy();
    this->maxSearchDepth   = -1;
    this->gameEnd          = false;
    this->color            = UTILITY::COLOR_UNKOWN;
    this->fixSearchTime    = -1;
    this->remainSearchTime = -1;
    this->stateVisited.clear();
    this->transTable.clear();
    cout << "[set board maxBoardSize to " << size << "]" << endl;

    // Debug info
    // cout << "Ataxx Init Size" << endl;
    // cout << this->gameState->GetSize() << endl;
}

// Function set current player color
void Ataxx::SetCurrentPlayer(char color)
{
    if (this->gameState == NULL)
    {
        cout << "[illegal action - board size unknown]" << endl;
    }
    else
    {
        this->color = color;
        this->gameState->SetColorForNextMove(color);
        cout << "[Next Player Color: " << color << " ]" << endl;
    }
}

// Move Current Player
void Ataxx::MoveCurrentPlayer(int col,
                              int row,
                              int new_col,
                              int new_row)
{
    if (this->gameState == NULL)
    {
        cout << "[illegal action - board size unknown]" << endl;
    }
    else if (this->gameEnd)
    {
        cout << "[illegal action - Game has Finished]" << endl;
    }
    else
    {
        // game state use i and j
        // i is row
        // j is col
        this->gameHistory.push(this->gameState);

        if (this->IsLegalLocation(this->size - 1 - row)
            && this->IsLegalLocation(col)
            && this->IsLegalLocation(this->size - 1 - new_row)
            && this->IsLegalLocation(new_col)
            )
        {
            this->gameState = gameState->MoveToChild(this->size - 1 - row,
                                                     col,
                                                     this->size - 1 - new_row,
                                                     new_col);
        }
        else
        {
            cout << "[illegal action]" << endl;
            return;
        }
        this->gameState->PrintGameBoard();

        // Update visited state count
        this->stateVisited[this->gameState->GetHash()]++;
        this->UpdateEndGameFlag();

        cout << "[Next Player Color: " <<
            this->gameState->GetColorForNextMove() << " ]" << endl;

        if (this->gameEnd)
        {
            cout << this->gameState->GetEndGameString() << endl;
        }
    }
}

void Ataxx::SetUpBoard(istream& istream)
{
    if (this->gameEnd)
    {
        cout << "[illegal action - game has finished]" << endl;
    }

    if (this->gameState == NULL)
    {
        cout << "[illegal action - board size unknown]" << endl;
    }
    else
    {
        this->gameState->SetUpBoard(istream);
        cout << "[Next Player Color: " <<
            this->gameState->GetColorForNextMove() << " ]" << endl;

        // Update visited state count
        this->stateVisited[this->gameState->GetHash()]++;
        this->UpdateEndGameFlag();

        if (this->gameEnd)
        {
            cout << this->gameState->GetEndGameString() << endl;
        }
    }
}

void Ataxx::PrintGameBoard()
{
    this->gameState->PrintGameBoard();
}

void Ataxx::SetSearchSetting(const int setting)
{
    if (this->gameEnd)
    {
        cout << "[illegal action - game has finished]" << endl;
    }
    else if (this->gameState == NULL)
    {
        cout << "[illegal action - board size unknown]" << endl;
    }
    else
    {
        this->searchSetting = setting;

        if (this->searchSetting == this->SearchSetting_Fixed)
        {
            cout << "[mode 1]" << endl;
        }
        else if (this->searchSetting == this->SearchSettings_Advanced)
        {
            cout << "[mode 2]" << endl;
        }
        else
        {
            cout << "[mode unknown]" << endl;
        }
    }
}

void Ataxx::SetSearchDepth(const int depth)
{
    if (this->gameEnd)
    {
        cout << "[illegal action - game has finished]" << endl;
    }
    else if (this->gameState == NULL)
    {
        cout << "[illegal action - board size unknown]" << endl;
    }
    else
    {
        this->maxSearchDepth = depth;
        cout << "[search depth " << depth << "]" <<  endl;
    }
}

string Ataxx::FindBestMove()
{
    string move = "move: ";

    if (this->gameEnd)
    {
        return "[illegal action - game has finished]";
    }
    else if (this->gameState == NULL)
    {
        return "[illegal action - board size unknown]";
    }
    else if (this->gameState->GetColorForNextMove() == UTILITY::COLOR_UNKOWN)
    {
        return "[illegal action - current player color unknown]";
    }
    else if ((this->maxSearchDepth == -1) &&
             (this->fixSearchTime == -1) &&
             (this->remainSearchTime == -1))
    {
        return "[illegal action - unknown search parameter]";
    }

    if (this->searchSetting == this->SearchSetting_UNKNOWN) {
        return "[illegal action - unknown search setting]";
    }
    else
    {
        this->gameHistory.push(this->gameState);
        try {
            clock_t statTime    = clock();
            int     timeAllowed = this->fixSearchTime;

            if (this->remainSearchTime > 0)
            {
                timeAllowed = (int)ceil (this->remainSearchTime / 10.0);

                if (timeAllowed > 10)
                {
                    timeAllowed = 10;
                }
                this->remainSearchTime -= timeAllowed;
                cout << "[Time allocated: " << timeAllowed <<  "]" << endl;
            }
            this->gameState = this->gameState->FindBestMove(this->maxSearchDepth,
                                                            this->transTable,
                                                            this->stateVisited,
                                                            statTime,
                                                            timeAllowed,
                                                            this->searchSetting);
        }
        catch (AtaxxState *state)
        {
            this->gameState = state;
        }
        move += this->gameState->GetLastMoveStr();
    }

    // update state count
    this->stateVisited[this->gameState->GetHash()]++;
    this->UpdateEndGameFlag();
    this->gameState->PrintGameBoard();
    cout << "[Next Player Color: " << this->gameState->GetColorForNextMove() <<
        " ]" << endl;

    if (this->gameEnd)
    {
        cout << this->gameState->GetEndGameString() << endl;
    }
    return move;
}

void Ataxx::InitZobrist()
{
    this->CleanZobristTable();

    random_device rd;
    mt19937_64    gen(rd());

    uniform_int_distribution<unsigned long long> dis;
    this->zobrist = new unsigned long long **[maxBoardSize];


    for (int i = 0; i < this->maxBoardSize; ++i)
    {
        this->zobrist[i] = new unsigned long long *[maxBoardSize];

        for (int j = 0; j < this->maxBoardSize; ++j)
        {
            this->zobrist[i][j] = new unsigned long long[numColor];

            for (int k = 0; k < this->numColor; ++k)
            {
                this->zobrist[i][j][k] = dis(gen);
            }
        }
    }
}

void Ataxx::Undo()
{
    if (this->gameEnd)
    {
        cout << "[illegal action - game has finished]" << endl;
    }
    else if (!this->gameHistory.empty() && (this->gameState != NULL))
    {
        delete this->gameState;
        this->gameState = this->gameHistory.top();
        this->gameHistory.pop();

        cout << "[Undo By One Step]" << endl;
        this->PrintGameBoard();
    }
    else
    {
        cout << "[illegal action - can not undo]" << endl;
    }
}

void Ataxx::Debug()
{
    this->gameState->PrintInfo();
}

void Ataxx::CleanHistroy()
{
    while (!this->gameHistory.empty())
    {
        delete this->gameHistory.top();
        this->gameHistory.pop();
    }
}

void Ataxx::SetFixSearchTime(const int time)
{
    this->fixSearchTime = time;
    cout << "[Fixed Search Time - " << this->fixSearchTime << "]" << endl;
}

void Ataxx::SetRemainSearchTime(const int time)
{
    this->remainSearchTime = time;
    cout << "[Total Search Time - " << this->remainSearchTime << "]" << endl;
}

void Ataxx::CleanZobristTable() {
    if (this->zobrist != NULL)
    {
        for (int i = 0; i < this->maxBoardSize; i++)
        {
            for (int j = 0; j < this->maxBoardSize; j++)
            {
                delete[] this->zobrist[i][j];
            }
            delete[] this->zobrist[i];
        }
        delete[] this->zobrist;
        this->zobrist = NULL;
    }
}

void Ataxx::UpdateEndGameFlag()
{
    bool exceedRepeatLimit = this->stateVisited[this->gameState->GetHash()] >
                             this->MaxAllowedRepeatState;
    bool exceededJumpLimit = this->gameState->ExceededJumpLimit();
    bool noOnehasMove      = this->gameState->BothSideHadNoMoreMove();

    // cout << exceedRepeat << "  " << exceededJumpLimit << " " << noOnehasMove;
    this->gameEnd = exceedRepeatLimit  || exceededJumpLimit || noOnehasMove;
}

bool Ataxx::IsLegalLocation(int i)
{
    return 0 <= i && i < this->size;
}
