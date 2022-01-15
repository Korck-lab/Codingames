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

char *makeStr(char output[], byte input, bool inv)
{
    char l = inv ? 'R' : 'L';
    char r = inv ? 'L' : 'R';
    for (short p = 0; p < 4; p++)
    {
        output[p * 2] = (input & 0x1 << (3 - p)) ? r : l;
    }

    return output;
}

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
    bool Fa = (state & F); //1 0 0 0
    bool Wa = (state & W); //1 0 0 0
    bool Ga = (state & G); //1 0 1 0
    bool Ca = (state & C); //1 1 0 0
    bool mF = !Fa;         //always switch sides
    bool mG = Ga == Fa ? ((Wa == Ga && Ca == Ga) || (Wa != Ga && Ca != Ga)) || (Wa != Ca): Ga;
    bool mC = (Ca == Fa && !Ca && mG == Ga) || Ca;
    bool mW = (Wa == Fa && !Wa && mC == Ca && mG == Ga) || Wa;
    byte nextState = (F * mF) | (W * mW) | (G * mG) | (C * mC);
    char b[] = "? ? ? ?\n";
    printf(makeStr(b, nextState, false));

    // fprintf(stderr, "%x %x \n", state, nextState);

    return nextState;
}

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
    if(init == target)
    {
        printf(TARGET);
        return 0;
    }
    bool inv = init&W+init&C > target&W+target&C;
    if (inv) // XOR
    {
        init = init^0xF;
        target = target^0xF;
    }
    short maxP = 4 * 3 * 2 / (4 - 2);
    byte P[maxP];
    P[0] = init;
    short c=0;
    for (int i = 0; i < maxP - 1; i++)
    {
        P[i + 1] = crossStep(P[i]);
        c++;
        if (P[i+1] == target)
        {
            // fprintf(stderr, "end state\n");
            break;
        }
    }
    char output[] = "? ? ? ?\n";
    for(int i=0; i<=c; i++)
    {
        printf(makeStr(output, P[i], inv));
    }

    // Write an answer using printf(). DON'T FORGET THE TRAILING \n
    // To debug: fprintf(stderr, "Debug messages...\n");

    // printf("L L L L\n");
    // printf("R L R L\n");

    return 0;
}