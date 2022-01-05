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
#define MAXEVAL 8
#define STONES 40
#define MAXDEPTH 4
#define BOARDSIZE 9

class Play
{

public:
  int x;
  int y;
  int score;
  Play(int x = -1, int y = -1, int score = INT16_MIN)
      : x(x), y(y), score(score){};
  bool operator<(const Play &m) const
  {
    return ((y * 9 + x) < (m.y * 9 + m.x));
    // return ((const void*)this < (const void*)&m);
  }
  //   friend bool operator<(const Play &m1, const Play &m2){
  //     return ((m1.y*9+m1.x) < (m2.y*9+m2.x));
  //   }
  //   friend bool operator>(const Play &m1, const Play &m2){
  //       return(m1.score>m2.score);
  //   }
  void dbg()
  {
    cerr << "x[" << x << "] y[" << y << "] score[" << score << "]" << endl;
  }
};

int score(bool captures[BOARDSIZE][BOARDSIZE], const char board[BOARDSIZE][BOARDSIZE], char myC)
{
  auto x=0;
  for (int x = 0; x < BOARDSIZE; x++)
  {
    for (int y = 0; y < BOARDSIZE; y++)
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
    for (int c = 0; c < BOARDSIZE * BOARDSIZE; c++)
    {
      int x = c % BOARDSIZE;
      int y = c / BOARDSIZE;

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

  int myCount = 0;
  for (int x = 0; x < BOARDSIZE; x++)
  {
    for (int y = 0; y < BOARDSIZE; y++)
    {
      myCount += (captures[y][x] == false && board[y][x] != myC)
                     ? 1
                 : (captures[y][x] == false && board[y][x] == myC) ? -1
                                                                   : 0;
    }
  }
  return (myCount);
}

Play searchMove(char oboard[BOARDSIZE][BOARDSIZE], bool ocaptures[BOARDSIZE][BOARDSIZE], int wc, int bc,
                char myC, char opC, bool myMove, int depth, int maxDepth)
{
  //   cerr << depth << " | entrou! " <<endl;
  //   return(Play());

  //MAKE OWN COPY & APPLY CAPTURES
  int free = 0;
  char board[BOARDSIZE][BOARDSIZE];
  bool captures[BOARDSIZE][BOARDSIZE];
  for (int i = 0; i < BOARDSIZE; i++)
  {
    for (int j = 0; j < BOARDSIZE; j++)
    {
      if (board[i][j] == A)
        free++;
      board[i][j] = ocaptures[i][j] == false ? A : oboard[i][j];
      captures[i][j] = true;
      if (board[i][j] == A)
        free++;
    }
  }

  vector<Play> possibles;
  //   map<Play, int> scores;
  int eval = 0;
  for (int i = 0; i < BOARDSIZE * BOARDSIZE; ++i)
  {
    if (eval >= min(MAXEVAL, free))
    {
      break;
    }
    int x = i % BOARDSIZE; // rand() % 9;
    int y = i / BOARDSIZE; // rand() % 9;

    char p = board[y][x];
    if (p != A)
    {
      // cerr << depth << " | skiping " << board[y][x] << " | "<<endl;
      continue; // taken
    }
    Play m(x, y);
    // cerr << depth << " | board value " << board[y][x] << " | ";
    // m.dbg();
    // if (scores.find(m) != scores.end()) {
    //     cerr << depth << " | skiping ";
    //     m.dbg();
    //   continue; // already evaluated
    // }

    // check if possible to play here
    char color = myMove ? myC : opC;
    char invColor = !myMove ? myC : opC;

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
    // cerr << "eval score..." << endl;
    m.score = score(captures, board, myC);
    // cerr << " ==> score" << m.score << endl;

    if (depth != maxDepth)
    {
      Play m1 = searchMove(board, captures, wc, bc, myC, opC, !myMove, depth + 1, maxDepth);
      m.score += m1.score;
    }
    // m.dbg();
    // scores[m] = m.score;
    // cerr << depth << " | possible pos "
    //        << " + ";
    // m.dbg();
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
  }
  else //optimize for the other player
  {
    sort(possibles.begin(), possibles.end(),
         [](const auto &l, const auto &r)
         { return (l.score < r.score); });
  }

  if (!possibles.empty())
  {
    int c = 0;
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
  int board_size = BOARDSIZE;
  cin >> board_size; // the size of the board (width and height)
  cin.ignore();
  //   cerr << "Aqui1" << endl;

  char board[BOARDSIZE][BOARDSIZE]; //={{'.', '.', '.', '.', '.', '.', '.', '.', '.'},{'.', '.', '.', '.', '.', '.', '.', '.', '.'},{'.', '.', '.', '.', '.', '.', '.', '.', '.'},{'.', '.', '.', '.', '.', '.', '.', '.', '.'},{'.', '.', '.', '.', '.', '.', '.', '.', '.'},{'.', '.', '.', '.', '.', '.', '.', '.', '.'},{'.', '.', '.', '.', '.', '.', '.', '.', '.'},{'.', '.', '.', '.', '.', '.', '.', '.', '.'},{'.', '.', '.', '.', '.', '.', '.', '.', '.'},{'.', '.', '.', '.', '.', '.', '.', '.', '.'}};
  bool captures[BOARDSIZE][BOARDSIZE];
  for (int i = 0; i < BOARDSIZE; i++)
  {
    for (int j = 0; j < BOARDSIZE; j++)
    {

      board[i][j] = A;
      captures[i][j] = true;
    }
  }
  //   cerr << "Aqui2" << endl;

  int wc = 0;
  int bc = 0;
  char myC = my_color[0];
  char opC = myC == W ? B : W;

  // game loop
  while (1)
  {
    int opponent_x; // The x coordinate of your opponent's last move (or -1 if
                    // the opponent passed)
    int opponent_y; // The y coordinate of your opponent's last move (or -1 if
                    // the opponent passed)

    cin >> opponent_x >> opponent_y;
    cin.ignore();
    if (opponent_x != -1)
    {
      board[opponent_y][opponent_x] = opC;
    }
    int my_score;       // Your current score (the number of captured stones)
    int opponent_score; // the opponents current score (the number of captured
                        // stones)
    cin >> my_score >> opponent_score;
    cin.ignore();
    vector<string> vboard;
    for (int i = 0; i < board_size; i++)
    {
      string line;
      getline(
          cin,
          line); // A row of the current board where 'B' marks a black stone,
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
      cout << "PASS" << endl;
      continue;
    }
    board[max.y][max.x] = myC;
    cout << max.x << " " << max.y << endl;
  }
}