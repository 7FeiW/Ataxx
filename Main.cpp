#include "Ataxx.h"
#include <iostream>
#include <string>

using namespace std;

int main()
{
    string cmd;
    Ataxx  ataxx;

    while (cin >> cmd)
    {
        if (cmd == "i")
        {
            int size;
            cin >> size;
            ataxx.Init(size);
        }
        else if (cmd == "b")
        {
            ataxx.SetCurrentPlayer('b');
        }
        else if (cmd == "w")
        {
            ataxx.SetCurrentPlayer('w');
        }

        else if (cmd == "s")
        {
            ataxx.SetUpBoard(cin);
        }
        else if (cmd[0] == 'm')
        {
            int col    = cmd[1] - 'a';
            int row    = cmd[2] - '1';
            int newCol = cmd[3] - 'a';
            int newRow = cmd[4] - '1';
            ataxx.MoveCurrentPlayer(col, row, newCol, newRow);
            ataxx.PrintGameBoard();
        }
        else if (cmd == "u")
        {
            ataxx.Undo();
        }
        else if (cmd == "1")
        {
            ataxx.SetSearchSetting(ataxx.SearchSetting_Fixed);
        }
        else if (cmd == "2")
        {
            ataxx.SetSearchSetting(ataxx.SearchSettings_Advanced);
        }
        else if (cmd == "d")
        {
            int depth;
            cin >> depth;
            ataxx.SetSearchDepth(depth);
        }
        else if (cmd == "ft")
        {
            int time;
            cin >> time;
            ataxx.SetFixSearchTime(time);
        }
        else if (cmd == "rt")
        {
            int time;
            cin >> time;
            ataxx.SetRemainSearchTime(time);
        }
        else if (cmd == "g")
        {
            cout << ataxx.FindBestMove() << endl;
            cout.setf(std::ios_base::unitbuf);
        }
        else if (cmd == "q")
        {
            cout << "[quit]" << endl;
            break;
        }
        else if (cmd == "debug")
        {
            ataxx.Debug();
        }
        else
        {
            cout << "unknow input" << endl;
        }
    }
    return 0;
}
