#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <time.h>

using namespace std;

#define W 'W'
#define B 'B'
#define A '.'
#define P (string) "PASS"
#define MAXEVAL (short)20
#define MINEVAL (short)15
#define MAXDEPTH (short)2
#define BOARDSIZE (short)9
#define HBOARD (short)50
#define CAPTURE (float)1.0f
#define LOSE (float)-1.0f
#define TLIMIT (double)0.095

static int evals = 0;
clock_t tStart = clock();
char pattern1[][BOARDSIZE]={
'.i.......',
'ha.......',
'.b.......',
'gc.......',
'.d.......',
'.e.......',
'kf.......',
'jl.......',
'.m.......',
};

char pattern2[][BOARDSIZE]={
'.fe......',
'g.d......',
'cba......',
'.........',
'.........',
'.........',
'.........',
'.........',
'.........',
};

char pattern3[][BOARDSIZE]={
'.........',
'.........',
'..abd....',
'..c......',
'..e......',
'.........',
'.........',
'.........',
'.........',
};


class Play
{

public:
    short x;
    short y;
    short pos;
    float score;
    Play(short x = -1, short y = -1, float score = 0.0f, short pos = 0)
        : x(x), y(y), score(score), pos(0){};
    bool operator<(const Play &m) const
    {
        return (score > m.score); //reverse
    }
    Play swapCoordsCopy()
    {
        return(Play(y, x, score, pos));
    }
    void dbg()
    {
        cerr << "x[" << x << "] y[" << y << "] score[" << score << "] pos[" << pos << "] " << endl;
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
        }
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
            // cerr << captures[y][x];
            myCount += (captures[y][x] == false && board[y][x] != myC) ? CAPTURE : (captures[y][x] == false && board[y][x] == myC) ? LOSE
                                                                                                                                   : 0.0f;
        }
        // cerr << endl;
    }
    return (myCount);
}

Play searchMove(char oboard[BOARDSIZE][BOARDSIZE], bool ocaptures[BOARDSIZE][BOARDSIZE],
                vector<pair<short, short>> freePos, char myC, char opC, bool myMove,
                short depth, short maxDepth, short maxEval)
{
    // maxEval = maxEval < MINEVAL ? MINEVAL : maxEval;

    //MAKE OWN COPY & APPLY CAPTURES
    // short free = 0;
    char board[BOARDSIZE][BOARDSIZE];
    for (short i = 0; i < BOARDSIZE; i++)
    {
        for (short j = 0; j < BOARDSIZE; j++)
        {
            board[i][j] = ocaptures[i][j] == false ? A : oboard[i][j];
            //     if (board[i][j] == A)
            //     {
            //         free++;
            //     }
            // }
        }
    }
    //this level probability contribution
    float prob = 1.0f;
    if (depth > 1)
    {
        prob = (float)((BOARDSIZE * BOARDSIZE) - (freePos.size() - 1)) / (float)(BOARDSIZE * BOARDSIZE);
        // cerr << depth << " | prob local:"<< prob << " |";
        prob *= powf(prob, depth);
        // cerr <<"| prob global:"<< prob << " | " << endl;
    }

    vector<Play> possibles;
    bool captures[BOARDSIZE][BOARDSIZE];
    char color = myMove ? myC : opC;
    char invColor = !myMove ? myC : opC;

    short x, y, eval = 0;
    // int localEvals = maxEval < free ? maxEval : free;
    // cerr << depth << " | local evals:"<< localEvals << endl;

    for (auto pos = freePos.end() - 1; freePos.size() > 0 && pos >= freePos.begin(); pos--)
    {
        double tElapsed = double(clock() - tStart) / double(CLOCKS_PER_SEC);
        if (TLIMIT - tElapsed < 0.0)
        {
            cerr << "safe leaving search due time! " << tElapsed << " s" << endl;
            break;
        }
        // if (depth == 0)
        // {
        //     cerr << "Elapsed time " << tElapsed << " s" << endl;
        // }

        // if (eval >= localEvals)
        // {
        //     break;
        // }

        x = pos->second;
        y = pos->first;

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
        m.score = score(captures, board, myC) * prob;

        if (depth != maxDepth && freePos.size() >= 2)
        {
            vector<pair<short, short>> freePosChild(freePos);
            freePosChild.pop_back();
            Play m1 = searchMove(board, captures, freePosChild, myC, opC, !myMove, depth + 1, maxDepth, (maxEval / (depth + 1)) + 1);
            m.score += m1.score;
        }
        possibles.push_back(m);
        freePos.pop_back();

        // restore board for next simulation
        board[y][x] = p;
        eval++;
    }

    if (!possibles.empty())
    {
        if (myMove)
        {
            sort(possibles.begin(), possibles.end(),
                 [](const auto &l, const auto &r)
                 { return (l.score > r.score); });
        }
        else //optimize for the other player
        {
            sort(possibles.begin(), possibles.end(),
                 [](const auto &l, const auto &r)
                 { return (l.score < r.score); });
        }
        Play ret = possibles.front();
        if (depth == 0)
        {
            cerr << "best:";
            possibles.front().dbg();
            cerr << "worst:";
            possibles.back().dbg();
            return (ret.score < 0.0 ? Play() : ret);
        }
        return (ret);
    }
    //   cerr << depth << " | return PASS! " << endl;
    return (Play()); // pass
}


vector<vector<Play>> create_strategies(std::initializer_list<char(*)[BOARDSIZE]> patterns)
{
    vector<vector<Play>> res;
    for(auto blueprint:patterns)
    {
        vector<Play> pattern[8];
        for(short i = 0; i<BOARDSIZE; i++)
        {
            for(short j = 0; j<BOARDSIZE; j++)
            {
                Play where(i,j);
                Play wherex(i, BOARDSIZE-j-1); // x mirroed
                Play wherey(BOARDSIZE-i-1, j); // y mirroed
                Play wherexy(BOARDSIZE-i-1, BOARDSIZE-j-1); // x & y mirroed
                where.pos = short(blueprint[i][j])-short('a');
                wherex.pos = where.pos;
                wherey.pos = where.pos;
                wherexy.pos = where.pos;
                pattern[0].push_back(where);
                pattern[1].push_back(where.swapCoordsCopy());
                pattern[2].push_back(wherex);
                pattern[3].push_back(wherex.swapCoordsCopy());
                pattern[4].push_back(wherey);
                pattern[5].push_back(wherey.swapCoordsCopy());
                pattern[6].push_back(wherexy);
                pattern[7].push_back(wherexy.swapCoordsCopy());
            }           
        }
        for(short i = 0; i<8; i++)
            res.push_back(pattern[i]);
    }

    return(res);
}

void test()
{
    char inputstr[] =
        R"(.BBW.B.B.
BBBBB.B.B
WW.B.B.B.
...BBBBBW
BB.WB.BW.
..B.WBBWW
BBWW.WWWW
BW.WWWW.B
W.W.WWWW.)";

    char myC = B;
    char opC = myC == W ? B : W;

    char board[BOARDSIZE][BOARDSIZE];
    bool captures[BOARDSIZE][BOARDSIZE];
    short count = 0;
    vector<pair<short, short>> freePos;

    for (short i = 0; i < BOARDSIZE; i++)
    {
        for (short j = 0; j < BOARDSIZE; j++)
        {
            // string line;
            board[i][j] = (char)inputstr[count++];
            if (board[i][j] == A)
            {
                freePos.push_back(pair(i, j));
            }
            cerr << board[i][j];
            captures[i][j] = true;

            // getline(cin, line); // A row of the current board where 'B' marks a black stone,
            // 'W' marks a white stone and '.' marks an empty field
            // vboard.push_back(line);
        }
        count++; //skip \n
    }

    tStart = clock();

    Play max = searchMove(board, captures, freePos, myC, opC, true, 0, MAXDEPTH, MAXEVAL);
    cerr << "TEST: expected score:" << max.score << endl;
    cerr << "TEST: total evaluations:" << evals << endl;
}

/**
 * Capture more stones than your opponent.
 **/
static short opponent_x; // The x coordinate of your opponent's last move (or -1 if
                         // the opponent passed)
static short opponent_y; // The y coordinate of your opponent's last move (or -1 if
                         // the opponent passed)

auto patterns = {pattern1, pattern2, pattern3};
auto strategies = create_strategies(patterns);
vector<Play> currStrategy;
short strategyIndex=0;

int main()
{
    // test();

    // return(0);
    //randomize
    sort(strategies.begin(), strategies.end(), [](const auto &l, const auto &r)
    {
        return(rand()&0x1);
    });

    currStrategy = strategies.back();
    strategies.pop_back();


    string my_color;
    getline(cin, my_color);
    short board_size = BOARDSIZE;
    cin >> board_size; // the size of the board (width and height)
    cin.ignore();

    short wc = 0;
    short bc = 0;
    char myC = my_color[0];
    char opC = myC == W ? B : W;
    char board[BOARDSIZE][BOARDSIZE];
    bool captures[BOARDSIZE][BOARDSIZE];

    // game loop
    while (1)
    {
        tStart = clock();
        evals = 0;

        cin >> opponent_x >> opponent_y;
        cin.ignore();

        short my_score;       // Your current score (the number of captured stones)
        short opponent_score; // the opponents current score (the number of captured
                              // stones)
        cin >> my_score >> opponent_score;
        cin.ignore();
        vector<pair<short, short>> freePos;
        // vector<string> vboard;
        for (short i = 0; i < board_size; i++)
        {
            for (short j = 0; j < board_size; j++)
            {
                // string line;
                board[i][j] = (char)getchar();
                if (board[i][j] == A)
                {
                    freePos.push_back(pair(i, j));
                }
                cerr << board[i][j];
                captures[i][j] = true;

                // getline(cin, line); // A row of the current board where 'B' marks a black stone,
                // 'W' marks a white stone and '.' marks an empty field
                // vboard.push_back(line);
            }
            cin.ignore();
            cerr << endl;
        }
        if (opponent_x != -1)
        {
            //sort around player last move
            sort(freePos.begin(), freePos.end(),
                 [](const auto &l, const auto &r)
                 {
                     short ld = abs(opponent_y - l.first) + abs(opponent_x - l.second);
                     short rd = abs(opponent_y - r.first) + abs(opponent_x - r.second);
                     return (ld < rd);
                 });
        }

        // for (short i = 0; i < BOARDSIZE; i++)
        // {
        //     for (short j = 0; j < BOARDSIZE; j++)
        //     {
        //         board[i][j] = vboard[i][j];
        //         captures[i][j] = true;
        //     }
        // }
        Play max = searchMove(board, captures, freePos, myC, opC, true, 0, MAXDEPTH, MAXEVAL);
        cerr << "expected score:" << max.score << endl;
        cerr << "total evaluations:" << evals << endl;

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        // x and y coordinates of the stone you want to place or PASS if you don't
        // want to place a stone
        if (max.x == -1)
        {
            cout << P << endl;
            cerr << P << endl;
            continue;
        }
        board[max.y][max.x] = myC;
        cout << max.x << " " << max.y << endl;
    }
}