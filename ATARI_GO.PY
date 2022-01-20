import sys
import math
from time import time_ns
import numpy as np

games = 0
board = np.array([['.'*9], ['.'*9], ['.'*9], ['.'*9],
                 ['.'*9], ['.'*9], ['.'*9], ['.'*9], ['.'*9]])
max_time = 100
start = 0
start_pos = np.array([[2, 2], [6, 2], [2, 6], [6, 6]])

def suicide(x, y, op_color):
    global board
    
    if ((y > 0 and board[y - 1][x] == op_color) or y == 0) and \
        ((y < 9 - 1 and board[y + 1][x] == op_color) or y == 9 - 1) and \
        ((x > 0 and board[y][x - 1] == op_color) or x == 0) and \
        ((x < 9 - 1 and board[y][x + 1] == op_color) or x == 9 - 1):
            return False
    
    return True

def play(my_color, op_color, depth):
    global games, board, start, max_time
    if my_color == 'B' and games == 0:
        return start_pos[np.random.choice(4)]

    for y, row in enumerate(board):
        for x, p in enumerate(row):
            if p == op_color:
                (x > 0 and board[y][x-1] == my_color)
                (x < 8 and board[y][x+1] == my_color)
                (y > 0 and board[y-1][x] == my_color)
                (y < 8 and board[y+1][x] == my_color)
            
            # if p == op_color and ((x > 0 and board[y][x-1] == my_color) or x == 0) and ((x < 8 and board[y][x+1] == my_color) or x == 8)

    c_time = (time_ns()//1000000)-start
    print("current time millis:{}".format(c_time), file=sys.stderr, flush=True)

    return [-1, -1]


# Capture more stones than your opponent.

my_color = input()
op_color = 'W' if my_color == 'B' else 'B'
board_size = int(input())  # the size of the board (width and height)
# game loop
while True:
    start = time_ns()//1000000

    # opponent_x: The x coordinate of your opponent's last move (or -1 if the opponent passed)
    # opponent_y: The y coordinate of your opponent's last move (or -1 if the opponent passed)
    opponent_x, opponent_y = [int(i) for i in input().split()]
    # my_score: Your current score (the number of captured stones)
    # opponent_score: the opponents current score (the number of captured stones)
    my_score, opponent_score = [int(i) for i in input().split()]
    for i in range(board_size):
        # A row of the current board where 'B' marks a black stone, 'W' marks a white stone and '.' marks an empty field
        board[i, 0:9] = input()[0:9]
        # print("{}".format(board[i, 0:9]), file=sys.stderr, flush=True)

    p_x, p_y = play(my_color, op_color, 0)
    games += 1

    # x and y coordinates of the stone you want to place or PASS if you don't want to place a stone
    print("PASS") if p_x == -1 else print("{} {}".format(p_x, p_y))
