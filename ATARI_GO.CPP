#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

#define W 'W'
#define B 'B'
#define A '.'
#define P "PASS"
#define MAXEVAL (short)30
#define MAXDEPTH (short)2
#define BOARDSIZE (short)9
#define HBOARD (short)50

class Play
{

public:
    short x;
    short y;
    short score;
    Play(short x = -1, short y = -1, short score = INT16_MIN)
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

short score(bool captures[BOARDSIZE][BOARDSIZE], const char board[BOARDSIZE][BOARDSIZE], char myC)
{
    auto x = 0;
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

            if (y > 0)
            {
                if (board[y][x] == A || board[y - 1][x] == board[y][x])
                {
                    changed = captures[y - 1][x] == true ? false : true;
                    captures[y - 1][x] = true;
                }
            }
            if (y < BOARDSIZE - 1)
            {
                if (board[y][x] == A || board[y + 1][x] == board[y][x])
                {
                    changed = captures[y + 1][x] == true ? false : true;
                    captures[y + 1][x] = true;
                }
            }
            if (x > 0)
            {
                if (board[y][x] == A || board[y][x - 1] == board[y][x])
                {
                    changed = captures[y][x - 1] == true ? false : true;
                    captures[y][x - 1] = true;
                }
            }
            if (x < BOARDSIZE - 1)
            {
                if (board[y][x] == A || board[y][x + 1] == board[y][x])
                {
                    changed = captures[y][x + 1] == true ? false : true;
                    captures[y][x + 1] = true;
                }
            }
        }
    } while (changed);

    short myCount = 0;
    for (short x = 0; x < BOARDSIZE; x++)
    {
        for (short y = 0; y < BOARDSIZE; y++)
        {
            myCount += (captures[y][x] == false && board[y][x] != myC) ? 1 : (captures[y][x] == false && board[y][x] == myC) ? -1
                                                                                                                             : 0;
        }
    }
    return (myCount);
}

Play searchMove(char oboard[BOARDSIZE][BOARDSIZE], bool ocaptures[BOARDSIZE][BOARDSIZE], short wc, short bc,
                char myC, char opC, bool myMove, short depth, short maxDepth)
{

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

    vector<Play> possibles;
    bool captures[BOARDSIZE][BOARDSIZE];
    char color = myMove ? myC : opC;
    char invColor = !myMove ? myC : opC;

    short x, y, eval = 0;
    for (short i = 0; i < BOARDSIZE * BOARDSIZE; ++i)
    {
        if (eval >= min(MAXEVAL, free))
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
        m.score = score(captures, board, myC);
        // cerr << " ==> score" << m.score << endl;

        if (depth != maxDepth)
        {
            Play m1 = searchMove(board, captures, wc, bc, myC, opC, !myMove, depth + 1, maxDepth);
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
                return (ret);
            }
        }
    }
    //   cerr << depth << " | return PASS! " << endl;
    return (Play()); // pass
}

/**
 * Capture more stones than your opponent.
 **/

int main()
{
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
        Play max = searchMove(board, captures, wc, bc, myC, opC, true, 0, MAXDEPTH);
        cerr << "expected score:" << max.score << endl;

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