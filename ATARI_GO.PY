import sys
import math
from time import time_ns
import numpy as np

games = 0
board = np.array([['.', '.', 'W', '.', 'W', 'B', '.', '.', '.'],
                  ['.', '.', 'W', 'W', 'B', '.', '.', '.', '.'],
                  ['.', '.', '.', 'B', 'W', '.', 'B', '.', '.'],
                  ['.', '.', '.', '.', '.', '.', '.', '.', '.'],
                  ['.', '.', '.', '.', '.', '.', '.', '.', '.'],
                  ['.', '.', '.', '.', '.', '.', '.', '.', '.'],
                  ['.', '.', '.', '.', '.', '.', '.', '.', '.'],
                  ['.', '.', '.', '.', '.', '.', '.', '.', '.'],
                  ['.', '.', '.', '.', '.', '.', '.', '.', '.']])
max_time = 100
start = 0
start_pos = np.array([[2, 2], [6, 2], [2, 6], [6, 6]])
my_pieces = []
op_pieces = []


def score(m_x, m_y, color):
    l_board = np.array(board)
    l_board[m_y][m_x] = color
    liberty = np.ndarray(shape=l_board.shape, dtype=bool)
    liberty[l_board != '.'] = False
    liberty[l_board == '.'] = True

    changed = True

    while changed:
        changed = False
        for y in range(l_board.shape[0]):
            for x in range(l_board.shape[1]):

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

    for y in range(l_board.shape[0]):
        for x in range(l_board.shape[1]):
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


def must_play(x, y, op_color):
    global board

    if ((y > 0 and board[y - 1][x] == op_color) or y == 0) and \
        ((y < board.shape[0]-1 and board[y + 1][x] == op_color) or y == board.shape[0]-1) and \
        ((x > 0 and board[y][x - 1] == op_color) or x == 0) and \
            ((x < board.shape[1]-1 and board[y][x + 1] == '.') or x == board.shape[1]-1):
        return [x+1, y]

    if ((y > 0 and board[y - 1][x] == op_color) or y == 0) and \
        ((y < board.shape[0]-1 and board[y + 1][x] == op_color) or y == board.shape[0]-1) and \
        ((x > 0 and board[y][x - 1] == '.') or x == 0) and \
            ((x < board.shape[1]-1 and board[y][x + 1] == op_color) or x == board.shape[1]-1):
        return [x-1, y]

    if ((y > 0 and board[y - 1][x] == op_color) or y == 0) and \
        ((y < board.shape[0]-1 and board[y + 1][x] == '.') or y == board.shape[0]-1) and \
        ((x > 0 and board[y][x - 1] == op_color) or x == 0) and \
            ((x < board.shape[1]-1 and board[y][x + 1] == op_color) or x == board.shape[1]-1):
        return [x, y+1]

    if ((y > 0 and board[y - 1][x] == '.') or y == 0) and \
        ((y < board.shape[0]-1 and board[y + 1][x] == op_color) or y == board.shape[0]-1) and \
        ((x > 0 and board[y][x - 1] == op_color) or x == 0) and \
            ((x < board.shape[1]-1 and board[y][x + 1] == op_color) or x == board.shape[1]-1):
        return [x, y-1]

    return [-1, -1]


def check_pos(my_color, best, verified, min_x, max_x, min_y, max_y, x, y):
    if x+y*board.shape[0] not in verified:
        verified[x+y*board.shape[0]] = True
        min_x = x if x < min_x else min_x
        min_y = y if y < min_y else min_y
        max_x = x if x > max_x else max_x
        max_y = y if y > max_y else max_y
        best.append([[x, y], score(x, y, my_color)])
    return min_x, max_x, min_y, max_y


def check_edges(my_color, op_color, best, verified, min_x, max_x, min_y, max_y, x, y):
    if y > 0 and board[y - 1][x] == '.' and not suicide(x, y-1, op_color):
        min_x, max_x, min_y, max_y = check_pos(
            my_color, best, verified, min_x, max_x, min_y, max_y, x, y - 1)

    if y < board.shape[0]-1 and board[y + 1][x] == '.' and not suicide(x, y+1, op_color):
        min_x, max_x, min_y, max_y = check_pos(
            my_color, best, verified, min_x, max_x, min_y, max_y, x, y + 1)

    if x > 0 and board[y][x - 1] == '.' and not suicide(x-1, y, op_color):
        min_x, max_x, min_y, max_y = check_pos(
            my_color, best, verified, min_x, max_x, min_y, max_y, x - 1, y)

    if x < board.shape[1]-1 and board[y][x + 1] == '.' and not suicide(x+1, y, op_color):
        min_x, max_x, min_y, max_y = check_pos(
            my_color, best, verified, min_x, max_x, min_y, max_y, x + 1, y)

    return min_x, max_x, min_y, max_y


def play(my_color, op_color, depth):
    global games, board, start, max_time, my_pieces
    if my_color == 'B' and games == 0:
        return start_pos[np.random.choice(4)]
    # print("board:{}".format(board.shape), file=sys.stderr, flush=True)

    best = []
    verified = {}
    min_x = board.shape[1]
    max_x = 0
    min_y = board.shape[0]
    max_y = 0
    for x, y in op_pieces:
        min_x, max_x, min_y, max_y = check_edges(
            my_color, op_color, best, verified, min_x, max_x, min_y, max_y, x, y)

    for x, y in my_pieces:
        min_x, max_x, min_y, max_y = check_edges(
            my_color, op_color, best, verified, min_x, max_x, min_y, max_y, x, y)

    for y in range(min_y, max_y):
        for x in range(min_x, max_x):
            if board[y][x] != '.' or suicide(x, y, op_color):
                continue
            if x+y*board.shape[0] in verified:
                continue
            best.append([[x, y], score(x, y, my_color)])

    if len(best) > 0:
        best.sort(key=lambda x: x[1], reverse=True)

    return best[0][0] if best[0][1] >= 0.0 else [-1, -1]


# Capture more stones than your opponent.
debug = bool(len(sys.argv) > 1 and sys.argv[1] == "DEBUG")

if debug:
    print("debug mode ###", file=sys.stderr, flush=True)

my_color = input() if not debug else 'B'
op_color = 'W' if my_color == 'B' else 'B'
# the size of the board (width and height)
board_size = int(input()) if not debug else 9
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

    p_x, p_y = play(my_color, op_color, 0)
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
p_x, p_y = play(my_color, op_color, 0)
print("B = x:{} y:{}".format(p_x, p_y), file=sys.stderr, flush=True)

board[p_y][p_x] = my_color

for y in range(board.shape[0]):
    # A row of the current board where 'B' marks a black stone, 'W' marks a white stone and '.' marks an empty field
    print("{}".format(board[y, 0:9]), file=sys.stderr, flush=True)
