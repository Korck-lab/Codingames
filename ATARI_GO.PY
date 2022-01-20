import sys
import math
from time import time_ns
import numpy as np

games = 0
board = np.array([['.','.','.','.','.','.','.','.','.'],
                  ['.','.','.','.','.','.','.','.','.'],
                  ['.','.','.','.','.','.','.','.','.'],
                  ['.','.','.','.','.','.','.','.','.'],
                  ['.','.','.','.','.','.','.','.','.'],
                  ['.','.','.','.','.','.','.','.','.'],
                  ['.','.','.','.','.','.','.','.','.'],
                  ['.','.','.','.','.','.','.','.','.'],
                  ['.','.','.','.','.','.','.','.','.']])
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
    print("board:{}".format(board.shape), file=sys.stderr, flush=True)
    
    for y in range(9):
        for x in range(9):
            if board[y][x] == op_color:
                my = 0
                op = 0
                em = 0
                my += 1 if (x > 0 and board[y][x-1] == my_color) else 0
                my += 1 if (x < 8 and board[y][x+1] == my_color) else 0
                my += 1 if (y > 0 and board[y-1][x] == my_color) else 0
                my += 1 if (y < 8 and board[y+1][x] == my_color) else 0
                op += 1 if (x > 0 and board[y][x-1] == op_color) else 0
                op += 1 if (x < 8 and board[y][x+1] == op_color) else 0
                op += 1 if (y > 0 and board[y-1][x] == op_color) else 0
                op += 1 if (y < 8 and board[y+1][x] == op_color) else 0
                em += 1 if (x > 0 and board[y][x-1] == '.') else 0
                em += 1 if (x < 8 and board[y][x+1] == '.') else 0
                em += 1 if (y > 0 and board[y-1][x] == '.') else 0
                em += 1 if (y < 8 and board[y+1][x] == '.') else 0
                print("my:{} op:{} em:{}".format(my, op, em), file=sys.stderr, flush=True)

                if op == 0 and em >= 1:
                    if (x > 0 and board[y][x-1] == '.'):
                        return [x-1, y]
                    if (x < 8 and board[y][x+1] == '.'):
                        return [x+1, y]
                    if (y > 0 and board[y-1][x] == '.'):
                        return [x, y-1]
                    if (y < 8 and board[y+1][x] == '.'):
                        return [x, y+1]
                    
                    
            
            # if p == op_color and ((x > 0 and board[y][x-1] == my_color) or x == 0) and ((x < 8 and board[y][x+1] == my_color) or x == 8)

    return [-1, -1]


# Capture more stones than your opponent.
print("board:{}".format(board.shape), file=sys.stderr, flush=True)

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
    for y in range(board_size):
        # A row of the current board where 'B' marks a black stone, 'W' marks a white stone and '.' marks an empty field
        inp = input()
        for x in range(len(inp)):
            board[y][x] = inp[x]
        print("{}".format(board[y, 0:9]), file=sys.stderr, flush=True)

    p_x, p_y = play(my_color, op_color, 0)
    games += 1
    
    c_time = (time_ns()//1000000)-start
    print("current time millis:{}".format(c_time), file=sys.stderr, flush=True)
    
    # x and y coordinates of the stone you want to place or PASS if you don't want to place a stone
    print("PASS") if p_x == -1 else print("{} {}".format(p_x, p_y))
