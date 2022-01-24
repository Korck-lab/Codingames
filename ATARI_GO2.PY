import sys
import math
from time import time_ns
import numpy as np

max_depth = 2
games = 0
board = np.array([['.', '.', '.', '.', '.', '.', '.', '.', '.'],
                  ['.', '.', '.', '.', 'W', '.', '.', '.', '.'],
                  ['.', '.', '.', '.', '.', '.', '.', '.', '.'],
                  ['.', '.', '.', '.', '.', '.', '.', '.', '.'],
                  ['.', '.', '.', '.', '.', '.', '.', '.', '.'],
                  ['.', '.', '.', '.', '.', '.', '.', '.', '.'],
                  ['.', '.', '.', '.', '.', '.', 'B', '.', '.'],
                  ['.', '.', '.', '.', '.', '.', '.', '.', '.'],
                  ['.', '.', '.', '.', '.', '.', '.', 'W', 'B']])

max_time = 100
start = 0
start_pos = np.array([[[2, 2], 0.0], [[6, 2], 0.0],
                     [[2, 6], 0.0], [[6, 6], 0.0]])
my_pieces = []
op_pieces = []

bw = np.ones(shape=board.shape, dtype=float)
my_mask = np.array([[1.5, 1.0, 1.5],
                    [1.0, 0.0, 1.0],
                    [1.5, 1.0, 1.5]])
op_mask = np.array([[1.0, 1.5, 1.0],
                    [1.5, 0.0, 1.5],
                    [1.0, 1.5, 1.0]])


def score(y_range, x_range, m_x, m_y, color):

    l_board = np.array(board)
    l_board[m_y][m_x] = color
    liberty = np.ndarray(shape=l_board.shape, dtype=bool)
    liberty[l_board != '.'] = False
    liberty[l_board == '.'] = True

    # if m_x == 8 and m_y == 7 and color == 'W':
    #     print("here", file=sys.stderr, flush=True)

    changed = True

    while changed:
        changed = False
        # for y in range(l_board.shape[0]):
        #     for x in range(l_board.shape[0]):
        for y in range(max(y_range[0]-1, 0), min(y_range[1]+2, l_board.shape[0])):
            for x in range(max(x_range[0]-1, 0), min(x_range[1]+2, l_board.shape[1])):

                if not liberty[y][x]:
                    continue

                if y > 0:
                    if l_board[y][x] == '.' or l_board[y - 1][x] == l_board[y][x]:
                        changed = changed if liberty[y - 1][x] else True
                        liberty[y - 1][x] = True
                if y < l_board.shape[0]-1:
                    if l_board[y][x] == '.' or l_board[y + 1][x] == l_board[y][x]:
                        changed = changed if liberty[y + 1][x] else True
                        liberty[y + 1][x] = True
                if x > 0:
                    if l_board[y][x] == '.' or l_board[y][x-1] == l_board[y][x]:
                        changed = changed if liberty[y][x - 1] else True
                        liberty[y][x - 1] = True
                if x < l_board.shape[1]-1:
                    if l_board[y][x] == '.' or l_board[y][x + 1] == l_board[y][x]:
                        changed = changed if liberty[y][x + 1] else True
                        liberty[y][x + 1] = True

    my_count = 0

    # for y in range(l_board.shape[0]):
    #     for x in range(l_board.shape[1]):
    for y in range(max(y_range[0]-1, 0), min(y_range[1]+2, l_board.shape[0])):
        for x in range(max(x_range[0]-1, 0), min(x_range[1]+2, l_board.shape[1])):
            my_count += 1.0 if not liberty[y][x] and l_board[y][x] != color else \
                -1.0 if not liberty[y][x] and l_board[y][x] == color else 0.0

    return my_count


def suicide(x, y, op_color):
    global board

    if ((y > 0 and board[y - 1][x] == op_color) or y == 0) and \
        ((y < board.shape[0]-1 and board[y + 1][x] == op_color) or y == board.shape[0]-1) and \
        ((x > 0 and board[y][x - 1] == op_color) or x == 0) and \
            ((x < board.shape[1]-1 and board[y][x + 1] == op_color) or x == board.shape[1]-1):
        return True
    return False


def play(y_range, x_range, my_color, op_color, depth, score_factor):
    global games, board, start, max_time, my_pieces
    if my_color == 'B' and games == 0:
        return start_pos[np.random.choice(4)]

    best = []

    for y in range(y_range[0], y_range[1]):
        c_time = (time_ns()//1000000)-start
        if c_time > 100:
            break
        for x in range(x_range[0], x_range[1]):
            if board[y][x] != '.' or (board[y][x] == '.' and suicide(x, y, op_color)):
                continue

    if len(best) > 0:
        best.sort(key=lambda x: x[1],
                  reverse=True if score_factor >= 0.0 else False)

    return best[0] if len(best) > 0 else [[-1, -1], 0.0]


# Capture more stones than your opponent.
debug = bool(len(sys.argv) > 1 and sys.argv[1] == "DEBUG")

if debug:
    print("debug mode ###", file=sys.stderr, flush=True)

my_color = input() if not debug else 'B'
op_color = 'W' if my_color == 'B' else 'B'
# the size of the board (width and height)
board_size = int(input()) if not debug else 9
start = time_ns()//1000000
# game loop
while not debug:
    start = time_ns()//1000000
    my_pieces = []
    # opponent_x: The x coordinate of your opponent's last move (or -1 if the opponent passed)
    # opponent_y: The y coordinate of your opponent's last move (or -1 if the opponent passed)
    opponent_x, opponent_y = [int(i) for i in input().split()]
    # my_score: Your current score (the number of captured stones)
    # opponent_score: the opponents current score (the number of captured stones)
    my_score, opponent_score = [int(i) for i in input().split()]
    for y in range(board_size):
        # A row of the current board where 'B' marks a black stone, 'W' marks a white stone and '.' marks an empty field
        inp = input()
        for x in range(len(inp)):
            board[y][x] = inp[x]
            if inp[x] == my_color:
                my_pieces.append([x, y])
            if inp[x] == op_color:
                op_pieces.append([x, y])
        # print("{}".format(board[y, 0:9]), file=sys.stderr, flush=True)

    t = play((0, board.shape[0]), (0, board.shape[1]),
             my_color, op_color, 0, 1.0)
    # print(t, file=sys.stderr, flush=True)
    p_x = t[0][0]
    p_y = t[0][1]
    games += 1

    c_time = (time_ns()//1000000)-start
    print("current time millis:{}".format(c_time), file=sys.stderr, flush=True)

    # x and y coordinates of the stone you want to place or PASS if you don't want to place a stone
    print("PASS") if p_x == -1 else print("{} {}".format(p_x, p_y))

# debug block
for y in range(board.shape[0]):
    # A row of the current board where 'B' marks a black stone, 'W' marks a white stone and '.' marks an empty field
    for x in range(board.shape[1]):
        if board[y][x] == my_color:
            my_pieces.append([x, y])
        if board[y][x] == op_color:
            op_pieces.append([x, y])
    print("{}".format(board[y, 0:9]), file=sys.stderr, flush=True)
games = 5
t = play((0, board.shape[0]), (0, board.shape[1]), my_color, op_color, 0, 1.0)
c_time = (time_ns()//1000000)-start
print("current time millis:{}".format(c_time), file=sys.stderr, flush=True)
p_x = t[0][0]
p_y = t[0][1]
print("B = x:{} y:{}".format(p_x, p_y), file=sys.stderr, flush=True)

board[p_y][p_x] = my_color

for y in range(board.shape[0]):
    # A row of the current board where 'B' marks a black stone, 'W' marks a white stone and '.' marks an empty field
    print("{}".format(board[y, 0:9]), file=sys.stderr, flush=True)
