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

class Play
{

public:
    short x;
    short y;
    short pos;
    float score;
    Play(short x = -1, short y = -1, float score = 0.0f, short pos = 0)
        : x(x), y(y), score(score), pos(0){};
    // bool operator<(const Play &m) const
    // {
    //     return (score > m.score); //reverse
    // }
    Play swapCoordsCopy()
    {
        return (Play(y, x, score, pos));
    }
    void dbg()
    {
        cerr << "x[" << x << "] y[" << y << "] score[" << score << "] pos[" << pos << "] " << endl;
    }
};

vector<vector<Play>> create_strategies(std::initializer_list<char (*)[BOARDSIZE]> patterns)
{
    vector<vector<Play>> res;
    for (auto blueprint : patterns)
    {
        vector<Play> pattern[8];
        for (short i = 0; i < BOARDSIZE; i++)
        {
            for (short j = 0; j < BOARDSIZE; j++)
            {
                Play where(i, j);
                Play wherex(i, BOARDSIZE - j - 1);                  // x mirroed
                Play wherey(BOARDSIZE - i - 1, j);                  // y mirroed
                Play wherexy(BOARDSIZE - i - 1, BOARDSIZE - j - 1); // x & y mirroed
                where.pos = blueprint[i][j] == '.' ? 0 : (short(blueprint[i][j]) - short('a'));
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
        for (short i = 0; i < 8; i++)
            res.push_back(pattern[i]);
    }

    return (res);
}

Play searchMove(char oboard[BOARDSIZE][BOARDSIZE], bool ocaptures[BOARDSIZE][BOARDSIZE],
                vector<pair<short, short>> freePos, char myC, char opC, bool myMove,
                short depth, short maxDepth, short maxEval)
{
    //MAKE OWN COPY & APPLY CAPTURES
    char board[BOARDSIZE][BOARDSIZE];
    for (short i = 0; i < BOARDSIZE; i++)
    {
        for (short j = 0; j < BOARDSIZE; j++)
        {
            board[i][j] = ocaptures[i][j] == false ? A : oboard[i][j];
        }
    }
    //this level probability contribution
    float prob = 1.0f;
    if (depth > 1)
    {
        prob = (float)((BOARDSIZE * BOARDSIZE) - (freePos.size() - 1)) / (float)(BOARDSIZE * BOARDSIZE);
        // cerr << depth << " | prob local:"<< prob << " |";
        prob *= powf(prob, (depth > 1 ? depth : 0));
        // cerr <<"| prob global:"<< prob << " | " << endl;
    }

    vector<Play> possibles;
    vector<Play> random;

    bool captures[BOARDSIZE][BOARDSIZE];
    char color = myMove ? myC : opC;
    char invColor = !myMove ? myC : opC;

    short x, y;

    for (auto pos = freePos.end() - 1; freePos.size() > 0 && pos >= freePos.begin(); pos--)
    {
        double tElapsed = double(clock() - tStart) / double(CLOCKS_PER_SEC);
        if (TLIMIT - tElapsed < 0.0)
        {
            cerr << "safe leaving search due time! " << tElapsed << " s" << endl;
            break;
        }

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
        if (m.score >= CAPTURE)
        {
            if (m.score >= CAPTURE * 2)
            {
                return (m);
            }
            possibles.push_back(m);
        }
        if (m.score > LOSE)
        {
            random.push_back(m);
        }

        freePos.pop_back();

        // restore board for next simulation
        board[y][x] = p;
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
        return (possibles.front());
        // Play ret = possibles.front();
        // if (depth == 0)
        // {
        //     cerr << "best:";
        //     possibles.front().dbg();
        //     cerr << "worst:";
        //     possibles.back().dbg();
        //     return (ret.score < 0.0 ? Play() : ret);
        // }
        // return (ret);
    }

    if (myMove && depth == 0 && !(currStrategy.empty() && strategies.empty()))
    {

        for (auto p = currStrategy.end() - 1; p >= currStrategy.begin(); p--)
        {
            Play m = *p;
            currStrategy.erase(p);
            if (currStrategy.empty())
            {
                if (strategies.empty())
                    break;
                currStrategy = strategies.back();
                strategies.pop_back();
            }
            if (board[m.y][m.x] != A)
            {
                continue;
            }
            cerr << "strategy move:";
            m.dbg();
            return (m);
        }
    }

    if (!random.empty())
    {
        return (random[rand() % random.size()]);
    }

    return (Play()); // pass
}
class Game;

class Agent
{
public:
    vector<vector<Play>> strategies;
    vector<Play> currStrategy;
    const char myC;
    Game *game;
    short captures;

    Agent(Game &game, const vector<vector<Play>> strategies, const char myC) : game(&game), myC(myC), captures(0)
    {
        this->strategies = vector<vector<Play>>(strategies);
        //randomize
        sort(strategies.begin(), strategies.end(), [](const auto &l, const auto &r)
             { return (bool(rand() & 0x1)); });

        currStrategy = strategies.back();
        this->strategies.pop_back();
    }

    Play searchMove(char lboard[BOARDSIZE][BOARDSIZE], bool lcaptures[BOARDSIZE][BOARDSIZE],
                    vector<pair<short, short>> lfreePos, short depth)
    {
        bool myMove = bool(depth % 2);
        char opC = myC == W ? B : W;
        char board[BOARDSIZE][BOARDSIZE];
        for (short i = 0; i < BOARDSIZE; i++)
        {
            for (short j = 0; j < BOARDSIZE; j++)
            {
                board[i][j] = lcaptures[i][j] == false ? A : lboard[i][j];
            }
        }
        //this level probability contribution
        float prob = 1.0f;
        if (depth > 1)
        {
            prob = (float)((BOARDSIZE * BOARDSIZE) - (lfreePos.size() - 1)) / (float)(BOARDSIZE * BOARDSIZE);
            // cerr << depth << " | prob local:"<< prob << " |";
            prob *= powf(prob, (depth > 1 ? depth : 0));
            // cerr <<"| prob global:"<< prob << " | " << endl;
        }

        vector<Play> possibles;
        vector<Play> random;

        bool captures[BOARDSIZE][BOARDSIZE];
        char color = myMove ? myC : opC;
        char invColor = !myMove ? myC : opC;

        short x, y;

        for (auto pos = lfreePos.end() - 1; lfreePos.size() > 0 && pos >= lfreePos.begin(); pos--)
        {
            double tElapsed = double(clock() - tStart) / double(CLOCKS_PER_SEC);
#ifndef TEST
            if (TLIMIT - tElapsed < 0.0)
            {
                cerr << "safe leaving search due time! " << tElapsed << " s" << endl;
                break;
            }
#endif
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
            if (m.score >= CAPTURE)
            {
                if (m.score >= CAPTURE * 2)
                {
                    return (m);
                }
                possibles.push_back(m);
            }
            if (m.score > LOSE)
            {
                random.push_back(m);
            }

            freePos.pop_back();

            // restore board for next simulation
            board[y][x] = p;
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
            return (possibles.front());
            // Play ret = possibles.front();
            // if (depth == 0)
            // {
            //     cerr << "best:";
            //     possibles.front().dbg();
            //     cerr << "worst:";
            //     possibles.back().dbg();
            //     return (ret.score < 0.0 ? Play() : ret);
            // }
            // return (ret);
        }

        if (myMove && depth == 0 && !(currStrategy.empty() && strategies.empty()))
        {

            for (auto p = currStrategy.end() - 1; p >= currStrategy.begin(); p--)
            {
                Play m = *p;
                currStrategy.erase(p);
                if (currStrategy.empty())
                {
                    if (strategies.empty())
                        break;
                    currStrategy = strategies.back();
                    strategies.pop_back();
                }
                if (board[m.y][m.x] != A)
                {
                    continue;
                }
                cerr << "strategy move:";
                m.dbg();
                return (m);
            }
        }

        if (!random.empty())
        {
            return (random[rand() % random.size()]);
        }

        return (Play()); // pass
    }
};

class Game
{
public:
    vector<pair<short, short>> freePos;
    char board[BOARDSIZE][BOARDSIZE];
    bool captures[BOARDSIZE][BOARDSIZE];

    Game()
    {
        initBoard();
    }

    bool play(Agent &p, Play m)
    {
        auto fp = freePos.begin();
        for (; fp < freePos.end(); fp++)
        {
            if (fp->first == m.x && fp->second == m.y)
            {
                break;
            }
        }

        if (fp == freePos.end())
        {
            return (false);
        }

        board[m.y][m.x] = p.myC;
        if (score(p) != 0.0)
        {
            for (short i; i < BOARDSIZE; i++)
            {
                for (short j; j < BOARDSIZE; j++)
                {
                    if (!captures[i][j])
                    {
                        board[m.y][m.x] = A;
                    }
                }
            }
        }

        // sort(freePos.begin(), freePos.end(),
        //      [](const auto &l, const auto &r)
        //      {
        //          return (bool(rand() & 0x1));
        //      });
    }

    void initBoard()
    {
        for (short i; i < BOARDSIZE; i++)
        {
            for (short j; j < BOARDSIZE; j++)
            {
                board[i][j] = '.';
                freePos.push_back(pair(i, j));
            }
        }
    }

    float score(Agent &p)
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

        float value = 0;
        for (short y = 0; y < BOARDSIZE; y++)
        {
            for (short x = 0; x < BOARDSIZE; x++)
            {
                // cerr << captures[y][x];
                value += (!captures[y][x] && board[y][x] != p.myC) ? CAPTURE : (!captures[y][x] && board[y][x] == p.myC) ? LOSE
                                                                                                                         : 0.0f;
            }
            // cerr << endl;
        }
        return (value);
    }
};

/**
 * Capture more stones than your opponent.
 **/

main()
{
    char pattern1[][BOARDSIZE] = {
        {'.', 'i', '.', '.', '.', '.', '.', '.', '.'},
        {'h', 'a', '.', '.', '.', '.', '.', '.', '.'},
        {'.', 'b', '.', '.', '.', '.', '.', '.', '.'},
        {'g', 'c', '.', '.', '.', '.', '.', '.', '.'},
        {'.', 'd', '.', '.', '.', '.', '.', '.', '.'},
        {'.', 'e', '.', '.', '.', '.', '.', '.', '.'},
        {'k', 'f', '.', '.', '.', '.', '.', '.', '.'},
        {'j', 'l', '.', '.', '.', '.', '.', '.', '.'},
        {'.', 'm', '.', '.', '.', '.', '.', '.', '.'}};

    char pattern2[][BOARDSIZE] = {
        {'.', 'f', 'e', '.', '.', '.', '.', '.', '.'},
        {'g', '.', 'd', '.', '.', '.', '.', '.', '.'},
        {'c', 'b', 'a', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.'}};

    char pattern3[][BOARDSIZE] = {
        {'.', '.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', 'a', 'b', 'd', '.', '.', '.', '.'},
        {'.', '.', 'c', '.', '.', '.', '.', '.', '.'},
        {'.', '.', 'e', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.'}};
    auto patterns = {pattern1, pattern2, pattern3};
    auto strategies = create_strategies(patterns);
    //randomize
    sort(strategies.begin(), strategies.end(), [](const auto &l, const auto &r)
         { return (bool(rand() & 0x1)); });

    string my_color = "B";

#ifndef TEST
    getline(cin, my_color);
    short board_size;
    cin >> board_size; // the size of the board (width and height)
    cin.ignore();
#endif

    char myC = my_color[0];
    char opC = myC == W ? B : W;

    Game game;

    Agent p1(game, strategies, myC);

    Agent p2(game, strategies, opC);

    // game loop
    while (1)
    {
        tStart = clock();
        evals = 0;

#ifndef TEST
        short opponent_x; // The x coordinate of your opponent's last move (or -1 if
                          // the opponent passed)
        short opponent_y; // The y coordinate of your opponent's last move (or -1 if
                          // the opponent passed)

        cin >> opponent_x >> opponent_y;
        cin.ignore();

        short my_score;       // Your current score (the number of captured stones)
        short opponent_score; // the opponents current score (the number of captured
                              // stones)
        cin >> my_score >> opponent_score;
        cin.ignore();
        // vector<string> vboard;
        game.freePos.clear();
        for (short i = 0; i < board_size; i++)
        {
            for (short j = 0; j < board_size; j++)
            {
                char t = (char)getchar();
                // game.board[i][j] = (char)getchar();
                // if (game.board[i][j] == A)
                // {
                //     game.freePos.push_back(pair(i, j));
                // }
                if (game.board[i][j] != t)
                {
                    cerr << "BOARD ERROR!! local:" << game.board[i][j] << " server:" << t << " coords: x:" << j << " y:" << i << endl;
                }
            }
            cin.ignore();
            cerr << endl;
        }

        if (opponent_x != -1)
        {
            game.play(p2, Play(opponent_x, opponent_y));
            //game.board[opponent_y][opponent_x] = opC;
        }
#endif

        Play max;
        try
        {
            max = searchMove(board, captures, freePos, myC, opC, true, 0, MAXDEPTH, MAXEVAL);
        }
        catch (exception &e)
        {
            cerr << e.what();
        }

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
        if (score(captures, board, myC) != 0.0)
        {
            for (short i = 0; i < board_size; i++)
            {
                for (short j = 0; j < board_size; j++)
                {
                    if (!captures[i][j])
                        board[i][j] = A;
                }
            }
        }
        cout << max.x << " " << max.y << endl;
    }
}