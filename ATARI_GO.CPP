#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <math.h>

using namespace std;

#define W 'W'
#define B 'B'
#define A '.'
#define P "PASS"
#define MAXEVAL (short)9
#define MINEVAL (short)6
#define MAXDEPTH (short)4
#define BOARDSIZE (short)9
#define HBOARD (short)50
#define CAPTURE (float)1.0f
#define LOSE (float)-2.0f

static int evals = 0;

class Play
{

public:
    short x;
    short y;
    float score;
    Play(short x = -1, short y = -1, float score = 0.0f)
        : x(x), y(y), score(score){};
    bool operator<(const Play &m) const
    {
        return (score > m.score); //reverse
    }
    void dbg()
    {
        cerr << "x[" << x << "] y[" << y << "] score[" << score << "]" << endl;
    }
};

float score(bool captures[BOARDSIZE][BOARDSIZE], const char board[BOARDSIZE][BOARDSIZE], char myC)
{
    evals++;
    for (short x = 0; x < BOARDSIZE; x++)
    {
        for (short y = 0; y < BOARDSIZE; y++)
        {
            captures[y][x] = board[y][x] == A ? true : false;
            //   cerr << m[y][x] << ".";
        }
        // cerr << endl;
    }

    bool changed;
    do
    {
        changed = false;
        for (short c = 0; c < BOARDSIZE * BOARDSIZE; c++)
        {
            short x = c % BOARDSIZE;
            short y = c / BOARDSIZE;

            if (!captures[y][x])
                continue;
            // cerr << "y:" << y << " x:" << x << " " << board[y][x] << endl;

            if (y > 0)
            {
                if (board[y][x] == A || board[y - 1][x] == board[y][x])
                {
                    changed = captures[y - 1][x] == true ? changed : true;
                    captures[y - 1][x] = true;
                    // if(board[y][x] != A)
                        // cerr << "changed:" << changed << " y:" << y-1 << " x:" << x << " " << board[y-1][x] << endl;
                }
            }
            if (y < BOARDSIZE - 1)
            {
                if (board[y][x] == A || board[y + 1][x] == board[y][x])
                {
                    changed = captures[y + 1][x] == true ? changed : true;
                    captures[y + 1][x] = true;
                }
            }
            if (x > 0)
            {
                if (board[y][x] == A || board[y][x - 1] == board[y][x])
                {
                    changed = captures[y][x - 1] == true ? changed : true;
                    captures[y][x - 1] = true;
                }
            }
            if (x < BOARDSIZE - 1)
            {
                if (board[y][x] == A || board[y][x + 1] == board[y][x])
                {
                    changed = captures[y][x + 1] == true ? changed : true;
                    captures[y][x + 1] = true;
                }
            }
        }
    } while (changed);

    float myCount = 0;
    for (short y = 0; y < BOARDSIZE; y++)
    {
        for (short x = 0; x < BOARDSIZE; x++)
        {
            cerr << captures[y][x];
            myCount += (captures[y][x] == false && board[y][x] != myC) ? CAPTURE : (captures[y][x] == false && board[y][x] == myC) ? LOSE
                                                                                                                                   : 0.0f;
        }
        cerr << endl;
    }
    return (myCount);
}

Play searchMove(char oboard[BOARDSIZE][BOARDSIZE], bool ocaptures[BOARDSIZE][BOARDSIZE], short wc, short bc,
                char myC, char opC, bool myMove, short depth, short maxDepth, short maxEval)
{
    maxEval = maxEval < MINEVAL ? MINEVAL : maxEval;

    //MAKE OWN COPY & APPLY CAPTURES
    short free = 0;
    char board[BOARDSIZE][BOARDSIZE];
    for (short i = 0; i < BOARDSIZE; i++)
    {
        for (short j = 0; j < BOARDSIZE; j++)
        {
            board[i][j] = ocaptures[i][j] == false ? A : oboard[i][j];
            if (board[i][j] == A)
            {
                free++;
            }
        }
    }

    //this level probability contribution
    float prob = (float)((BOARDSIZE * BOARDSIZE) - (free - 1)) / (float)(BOARDSIZE * BOARDSIZE);
    // cerr << depth << " | prob local:"<< prob << " |";
    prob *= powf(prob, depth);
    // cerr <<"| prob global:"<< prob << " | " << endl;

    vector<Play> possibles;
    bool captures[BOARDSIZE][BOARDSIZE];
    char color = myMove ? myC : opC;
    char invColor = !myMove ? myC : opC;

    short x, y, eval = 0;
    int localEvals = maxEval < free ? maxEval : free;
    // cerr << depth << " | local evals:"<< localEvals << endl;

    for (short i = 0; i < BOARDSIZE * BOARDSIZE; ++i)
    {

        if (eval >= localEvals)
        {
            break;
        }

        if (free >= HBOARD)
        {
            x = rand() % 9;
            y = rand() % 9;
        }
        else
        {
            x = i % BOARDSIZE; // rand() % 9;
            y = i / BOARDSIZE; // rand() % 9;
        }

        char p = board[y][x];
        if (p != A)
        {
            // cerr << depth << " | skiping " << board[y][x] << " | "<<endl;
            continue; // taken
        }

        // check if possible to play here

        if (((y > 0 && board[y - 1][x] == invColor) || y == 0) &&
            ((y < BOARDSIZE - 1 && board[y + 1][x] == invColor) || y == BOARDSIZE - 1) && // suicide check
            ((x > 0 && board[y][x - 1] == invColor) || x == 0) &&
            ((x < BOARDSIZE - 1 && board[y][x + 1] == invColor) || x == BOARDSIZE - 1))
        {
            //   cerr << depth << " | suicide pos "
            //        << " + ";
            //   m.dbg();
            continue; // didnt pass check
        }

        board[y][x] = color; // make move
        Play m(x, y);
        // cerr << "eval score..." << endl;
        m.score = score(captures, board, myC) * prob;
        // cerr << " ==> score" << m.score << endl;

        if (depth != maxDepth && free >= 2)
        {
            Play m1 = searchMove(board, captures, wc, bc, myC, opC, !myMove, depth + 1, maxDepth, (maxEval / (depth + 1)) + 1);
            m.score += m1.score;
        }
        possibles.push_back(m);

        // restore board for next simulation
        board[y][x] = p;
        eval++;
    }

    //   cerr << depth << " | possibles size " << possibles.size() << endl;
    if (myMove)
    {
        sort(possibles.begin(), possibles.end(),
             [](const auto &l, const auto &r)
             { return (l.score > r.score); });
        // cerr << depth << " | my best " << " + ";
        // possibles.front().dbg();
    }
    else //optimize for the other player
    {
        sort(possibles.begin(), possibles.end(),
             [](const auto &l, const auto &r)
             { return (l.score < r.score); });
        // cerr << depth << " | opponent best " << " + ";
        // possibles.front().dbg();
    }

    if (!possibles.empty())
    {
        short c = 0;
        for (auto p : possibles)
        {
            ++c;
            if (p.score < possibles.front().score || c == possibles.size())
            {
                Play ret = possibles[rand() % c];
                // cerr << depth << " | return play "
                //      << " + ";
                // ret.dbg();
                // if(depth == 0)
                // {
                //     //real score to next move
                //     ret.score = score(captures, board, myC);

                // }
                return (ret);
            }
        }
    }
    //   cerr << depth << " | return PASS! " << endl;
    return (Play()); // pass
}

void tests()
{
    // char board[BOARDSIZE][BOARDSIZE];
    bool captures[BOARDSIZE][BOARDSIZE];
    for (short i = 0; i < BOARDSIZE; i++)
    {
        for (short j = 0; j < BOARDSIZE; j++)
        {

            captures[i][j] = true;
        }
    }

    //BW.......
    //BW.......
    //W........
    //.........
    //.........
    //.........
    //.........
    //.........
    //.........
    //test one (my color = W => score 2)
    //test two (my color = B => score -4)
    char b1[][9] = {
        {
            B,
            W,
            A,
            A,
            A,
            A,
            A,
            A,
            A,
        },
        {
            B,
            W,
            A,
            A,
            A,
            A,
            A,
            A,
            A,
        },
        {
            W,
            A,
            A,
            A,
            A,
            A,
            A,
            A,
            A,
        },
        {
            A,
            A,
            A,
            A,
            A,
            A,
            A,
            A,
            A,
        },
        {
            A,
            A,
            A,
            A,
            A,
            A,
            A,
            A,
            A,
        },
        {
            A,
            A,
            A,
            A,
            A,
            A,
            A,
            A,
            A,
        },
        {
            A,
            A,
            A,
            A,
            A,
            A,
            A,
            A,
            A,
        },
        {
            A,
            A,
            A,
            A,
            A,
            A,
            A,
            A,
            A,
        },
        {
            A,
            A,
            A,
            A,
            A,
            A,
            A,
            A,
            A,
        }};
    float r1 = score(captures, b1, W);
    float r2 = score(captures, b1, B);
    cerr << "test 1.1 (my color = W => score 2):" << r1 << "| test 1.2 (my color = B => score -4):" << r2 << endl;
    if (r1 != 2.0f || r2 != -4.0f)
    {
        cerr << "!!!! TEST ERROR SCORE FUNCTION !!!!" << endl;
    }

    //.BW.B.B.B
    //.BBWBBBBB
    //W.WB.BWB.
    //....WBW..
    //..WW.W..B
    //WWBW.BB.B
    //....W.W.W
    //.BW.BW...
    //W.WWB.W.B
    //test one (my color = W => score 2)
    //test two (my color = B => score -4)
    char b2[][9] = {
        {
            A,
            B,
            W,
            A,
            B,
            A,
            B,
            A,
            B,
        },
        {
            A,
            B,
            B,
            W,
            B,
            B,
            B,
            B,
            B,
        },
        {
            W,
            A,
            W,
            B,
            A,
            B,
            W,
            B,
            A,
        },
        {
            A,
            A,
            A,
            A,
            W,
            B,
            W,
            A,
            A,
        },
        {
            A,
            A,
            W,
            W,
            A,
            W,
            A,
            A,
            B,
        },
        {
            W,
            W,
            B,
            W,
            A,
            B,
            B,
            A,
            B,
        },
        {
            A,
            A,
            A,
            A,
            W,
            A,
            W,
            A,
            W,
        },
        {
            A,
            B,
            W,
            A,
            B,
            W,
            A,
            A,
            A,
        },
        {
            W,
            A,
            W,
            W,
            B,
            A,
            W,
            A,
            B,
        }};
    float r3 = score(captures, b2, W);
    float r4 = score(captures, b2, B);
    cerr << "test 2.1 (my color = W => score 2):" << r3 << "| test 2.2 (my color = B => score -4):" << r4 << endl;
    if (r3 != 2.0f || r4 != -4.0f)
    {
        cerr << "!!!! TEST ERROR SCORE FUNCTION !!!!" << endl;
    }
}

/**
 * Capture more stones than your opponent.
 **/

int main()
{
    //must pass tests
    tests();

    string my_color;
    getline(cin, my_color);
    short board_size = BOARDSIZE;
    cin >> board_size; // the size of the board (width and height)
    cin.ignore();
    //   cerr << "Aqui1" << endl;

    char board[BOARDSIZE][BOARDSIZE];
    bool captures[BOARDSIZE][BOARDSIZE];
    for (short i = 0; i < BOARDSIZE; i++)
    {
        for (short j = 0; j < BOARDSIZE; j++)
        {

            board[i][j] = A;
            captures[i][j] = true;
        }
    }
    //   cerr << "Aqui2" << endl;

    short wc = 0;
    short bc = 0;
    char myC = my_color[0];
    char opC = myC == W ? B : W;

    // game loop
    while (1)
    {
        evals = 0;
        short opponent_x; // The x coordinate of your opponent's last move (or -1 if
                          // the opponent passed)
        short opponent_y; // The y coordinate of your opponent's last move (or -1 if
                          // the opponent passed)

        cin >> opponent_x >> opponent_y;
        cin.ignore();
        if (opponent_x != -1)
        {
            board[opponent_y][opponent_x] = opC;
        }
        short my_score;       // Your current score (the number of captured stones)
        short opponent_score; // the opponents current score (the number of captured
                              // stones)
        cin >> my_score >> opponent_score;
        cin.ignore();
        vector<string> vboard;
        for (short i = 0; i < board_size; i++)
        {
            string line;
            // cin.ignore();

            getline(cin, line); // A row of the current board where 'B' marks a black stone,
                                // 'W' marks a white stone and '.' marks an empty field
                                //   vboard.push_back(line);
                                // cerr << line << endl;
        }
        Play max = searchMove(board, captures, wc, bc, myC, opC, true, 0, MAXDEPTH, MAXEVAL);
        cerr << "expected score:" << max.score << endl;
        cerr << "total evaluations:" << evals << endl;

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        // x and y coordinates of the stone you want to place or PASS if you don't
        // want to place a stone
        if (max.x == -1)
        {
            cout << P << endl;
            continue;
        }
        board[max.y][max.x] = myC;
        cout << max.x << " " << max.y << endl;
    }
}