#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define byte unsigned char
#define F (0x1 << 3)
#define W (0x1 << 2)
#define G (0x1 << 1)
#define C (0x1 << 0)
#define L 0b0000
#define R 0b1111

byte makeByte(char *input)
{
    byte ret = 0;
    byte p = 3;
    for (char *c = input; *c != 0x0; c++)
    {
        if (*c == '\n')
            break;
        if (*c == ' ')
            continue;

        // fprintf(stderr, "%x %c %d\n", ret, *c, p);

        ret |= (*c == 'L' ? 0x0 : 0x1) << p;
        --p;
    }
    return ret;
}

byte crossStep(byte state)
{
    if(state == 0xf)
    {
        fprintf(stderr, "end state\n");
        return state;
    }
    byte Fa = (state & F); //1 0
    byte Wa = (state & W); //1 0
    byte Ga = (state & G); //1 0
    byte Ca = (state & C); //1 1
    byte mW = !Ga && !Ca;
    byte mG = true;
    byte mC = !Wa && !Ga;
    byte mF = (!Ga && !Ca) || (!Wa && !Ga);
    byte nextState = (F * mF) | (W * mW) | (G * mG) | (C * mC);

    fprintf(stderr, "%x %x \n", state, nextState);

    return nextState;
}
typedef struct state
{
    byte where;
    state *future[4];
} state;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{

    char INITIAL[11];
    scanf("%[^\n]", INITIAL);
    fgetc(stdin);
    char TARGET[11];
    scanf("%[^\n]", TARGET);
    byte init = makeByte(INITIAL);
    byte target = makeByte(TARGET);
    short maxP = 4 * 3 * 2 / (4 - 2);
    state P[maxP];
    memset(P, 0x0, maxP * sizeof(state));

    state *tree = &P[0];
    tree->where = init;
    state *curr = &P[0];
    for (int i = 0; i < maxP-1; i++)
    {
        P[i+1].where=crossStep(P[i].where);
    }

    // Write an answer using printf(). DON'T FORGET THE TRAILING \n
    // To debug: fprintf(stderr, "Debug messages...\n");

    printf("L L L L\n");
    printf("R L R L\n");

    return 0;
}