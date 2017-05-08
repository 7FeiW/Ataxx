//

// Created by Fei Wang on 2017/2/6.
//
#ifndef TTENTRY_H
#define TTENTRY_H

class TTEntry {
public:

    static const char UPPER    = 0;
    static const char LOWER    = 1;
    static const char ACCURATE = 2;
    int Score;
    int Height;

    // 0 , 1 , 2
    int Type;

    // Encode move int
    int BestMove;

    void Save(int score,
              int height,
              int alpha,
              int beta,
              int move);
};


#endif // TTENTRY_H
