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


def check_pos(my_color, op_color, best, verified, min_x, max_x, min_y, max_y, x, y, depth, score_factor):
    if x+y*board.shape[0] not in verified:
        verified[x+y*board.shape[0]] = True

        ld, yr, xr = liberty(my_color, y, x)

        min_x = xr[0] if xr[0] < min_x else min_x
        min_y = yr[0] if yr[0] < min_y else min_y
        max_x = xr[1] if xr[1] > max_x else max_x
        max_y = yr[1] if yr[1] > max_y else max_y

        if ld >= 2 and depth < max_depth:
            sc = score((min_y, max_y), (min_x, max_x),
                       x, y, my_color)*score_factor
            board[y][x] = my_color
            # swap player
            v = play((min_y, max_y), (min_x, max_x), op_color, my_color,
                     depth+1, -1.0*score_factor)
            # restore v_board
            board[y][x] = '.'
            best.append([[x, y], v[1]+sc])

        if ld >= 1 and depth == max_depth:
            best.append(
                [[x, y], score((min_y, max_y), (min_x, max_x), x, y, my_color)*score_factor])
    return min_x, max_x, min_y, max_y


def check_edges(my_color, op_color, best, verified, min_x, max_x, min_y, max_y, x, y, depth, score_factor):
    if y > 0 and board[y - 1][x] == '.' and not suicide(x, y-1, op_color):
        min_x, max_x, min_y, max_y = check_pos(
            my_color, op_color, best, verified, min_x, max_x, min_y, max_y, x, y - 1, depth, score_factor)

    if y < board.shape[0]-1 and board[y + 1][x] == '.' and not suicide(x, y+1, op_color):
        min_x, max_x, min_y, max_y = check_pos(
            my_color, op_color, best, verified, min_x, max_x, min_y, max_y, x, y + 1, depth, score_factor)

    if x > 0 and board[y][x - 1] == '.' and not suicide(x-1, y, op_color):
        min_x, max_x, min_y, max_y = check_pos(
            my_color, op_color, best, verified, min_x, max_x, min_y, max_y, x - 1, y, depth, score_factor)

    if x < board.shape[1]-1 and board[y][x + 1] == '.' and not suicide(x+1, y, op_color):
        min_x, max_x, min_y, max_y = check_pos(
            my_color, op_color, best, verified, min_x, max_x, min_y, max_y, x + 1, y, depth, score_factor)

    return min_x, max_x, min_y, max_y


def liberty(my_color, y, x):
    liberty = 0
    min_x = max(x-1, 0)
    max_x = min(x+1, board.shape[1]-1)+1
    min_y = max(y-1, 0)
    max_y = min(y+1, board.shape[0]-1)+1
    min_x = x if x < min_x else min_x
    min_y = y if y < min_y else min_y
    max_x = x if x > max_x else max_x
    max_y = y if y > max_y else max_y

    if y > 0 and (board[y - 1][x] == '.' or board[y - 1][x] == my_color):
        liberty += 1
        if board[y - 1][x] == my_color:
            t = np.where(board[:y, x:x+1] == my_color)
            if len(t) > 0 and t[0].shape[0] == 2:
                min_y = t[0][0] if t[0][0] < min_y else min_y
    if y < board.shape[0]-1 and (board[y + 1][x] == '.' or board[y + 1][x] == my_color):
        liberty += 1
        if board[y + 1][x] == my_color:
            t = np.where(board[y+1:, x:x+1] == my_color)
            if len(t) > 0 and t[0].shape[0] == 2:
                max_y = t[-1][0]+y+1 if t[-1][0]+y+1 > max_y else max_y
    if x > 0 and (board[y][x - 1] == '.' or board[y][x - 1] == my_color):
        liberty += 1
        if board[y][x - 1] == my_color:
            t = np.where(board[y:y+1, :x] == my_color)
            if len(t) > 0 and t[0].shape[0] == 2:
                min_x = t[0][1] if t[0][1] < min_x else min_x
    if x < board.shape[1]-1 and (board[y][x + 1] == '.' or board[y][x + 1] == my_color):
        liberty += 1
        if board[y][x + 1] == my_color:
            t = np.where(board[y:y+1, x+1:] == my_color)
            if len(t) > 0 and t[0].shape[0] == 2:
                max_x = t[-1][1]+x+1 if t[-1][1]+x+1 > max_x else max_x

    return liberty, (min_y, max_y), (min_x, max_x)


def play(y_range, x_range, my_color, op_color, depth, score_factor):
    global games, board, start, max_time, my_pieces
    if my_color == 'B' and games == 0:
        return start_pos[np.random.choice(4)]

    best = []

    verified = {}
    min_x = x_range[0]
    max_x = x_range[1]
    min_y = y_range[0]
    max_y = y_range[1]

    # for x, y in op_pieces:
    #     c_time=(time_ns()//1000000)-start
    #     if c_time > 100:
    #         break
    #     min_x, max_x, min_y, max_y = check_edges(
    #         my_color, op_color, best, verified, min_x, max_x, min_y, max_y, x, y, depth, score_factor)

    # for x, y in my_pieces:
    #     c_time=(time_ns()//1000000)-start
    #     if c_time > 100:
    #         break
    # # print("current time millis:{}".format(c_time), file=sys.stderr, flush=True)
    #     min_x, max_x, min_y, max_y = check_edges(
    #         my_color, op_color, best, verified, min_x, max_x, min_y, max_y, x, y, depth, score_factor)
        
    # for y in range(min_y, max_y):
    #     for x in range(min_x, max_x):
    #         if board[y][x] != '.' or suicide(x, y, op_color):
    #             continue
    #         if x+y*board.shape[0] in verified:
    #             continue
    #         best.append(
    #             [[x, y], score((min_y, max_y), (min_x, max_x), x, y, my_color)])

    for y in range(y_range[0], y_range[1]):
        c_time=(time_ns()//1000000)-start
        if c_time > 100:
            break
        for x in range(x_range[0], x_range[1]):
            if board[y][x] != '.' or (board[y][x] == '.' and suicide(x, y, op_color)):
                continue

            # if depth == max_depth:
            #     print("here", file=sys.stderr, flush=True)

            ld, yr, xr = liberty(my_color, y, x)

            if ld >= 2 and depth < max_depth:
                board[y][x] = my_color
                # swap player
                v = play(yr, xr, op_color, my_color,
                         depth+1, -1.0*score_factor)
                # restore v_board
                board[y][x] = '.'
                best.append([[x, y], v[1]])
                continue

            if ld >= 1 and not suicide(x, y, op_color) and depth == max_depth:
                sc = score(y_range, x_range, x, y, my_color)
                # sc *= score_factor
                best.append([[x, y], sc])

    if len(best) > 0:
        best.sort(key=lambda x: x[1],
                  reverse=True if score_factor >= 0.0 else False)
        # best[0][1] *= score_factor

    # c_time=(time_ns()//1000000)-start
    # print("current time millis:{}".format(c_time), file=sys.stderr, flush=True)

    return best[0] if len(best) > 0 else [[-1, -1], 0.0]

    # verified = {}
    # min_x = board.shape[1]
    # max_x = 0
    # min_y = board.shape[0]
    # max_y = 0
    # for x, y in op_pieces:
    #     min_x, max_x, min_y, max_y = check_edges(
    #         my_color, op_color, best, verified, min_x, max_x, min_y, max_y, x, y)

    # for x, y in my_pieces:
    #     min_x, max_x, min_y, max_y = check_edges(
    #         my_color, op_color, best, verified, min_x, max_x, min_y, max_y, x, y)

    # for y in range(min_y, max_y):
    #     for x in range(min_x, max_x):
    #         if board[y][x] != '.' or suicide(x, y, op_color):
    #             continue
    #         if x+y*board.shape[0] in verified:
    #             continue
    #         best.append([[x, y], score(x, y, my_color)])

    # if len(best) > 0:
    #     best.sort(key=lambda x: x[1][0], reverse=True)

    # return best[0] if best[0][1][0] >= 0.0 else [[-1, -1], [0.0, blank_liberty]]


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
