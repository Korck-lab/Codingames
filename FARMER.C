#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#define byte unsigned char
#define F (0x1 << 3)
#define W (0x1 << 2)
#define G (0x1 << 1)
#define C (0x1 << 0)
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
        ret |= (*c == 'L' ? 0x0 : 0x1) << p;
        --p;
    }
    return ret;
}
byte crossStep(byte state, bool invW)
{
    bool Fa = (state & F); //1 0 0 0
    bool Wa = (state & W); //1 0 0 0
    bool Ga = (state & G); //1 0 1 0
    bool Ca = (state & C); //1 1 0 0
    bool mF = !Fa;         //always switch sides
    bool mG = Fa == Ga ? (Wa < Ga && Ca < Ga) || (Wa > Ga && Ca > Ga) || (Wa == Ga && Ca == Ga) : Ga;
    bool mWi = (Wa == Fa && !Wa && mG == Ga) || Wa;
    bool mCi = (Ca == Fa && !Ca && mWi == Wa && mG == Ga) || Ca;
    bool mC = (Ca == Fa && !Ca && mG == Ga) || Ca;
    bool mW = (Wa == Fa && !Wa && mC == Ca && mG == Ga) || Wa;
    byte nextState = (F * mF) | (W * mW) | (G * mG) | (C * mC);
    byte nextStatei = (F * mF) | (W * mWi) | (G * mG) | (C * mCi);
    return invW ? nextStatei : nextState;
}

int solution(char output[][9], byte init, byte target)
{
    if (init == target)
    {
        char b[] = "? ? ? ?\n";
        printf(makeStr(b, init, false));
        return 0;
    }
    byte iinit = init ^ 0xF;
    byte itarget = target ^ 0xF;
    short maxP = 4 * 3 * 2 / (4 - 2);
    byte P0[maxP];
    byte P1[maxP];
    byte P2[maxP];
    byte P3[maxP];
    P0[0] = P2[0] = iinit;
    P1[0] = P3[0] = init;
    byte *P;
    short c = 0;
    bool inv = false;
    for (int i = 0; i < maxP - 1; i++)
    {
        P0[i + 1] = crossStep(P0[i], true);
        P1[i + 1] = crossStep(P1[i], false);
        P2[i + 1] = crossStep(P2[i], false);
        P3[i + 1] = crossStep(P3[i], true);
        c++;
        if (P0[i + 1] == itarget)
        {
            P = P0;
            break;
        }
        if (P1[i + 1] == target)
        {
            P = P1;
            break;
        }
        if (P2[i + 1] == itarget)
        {
            P = P2;
            inv = true;
            break;
        }
        if (P3[i + 1] == target)
        {
            P = P3;
            inv = true;
            break;
        }
    }
    char tmpo[] = "? ? ? ?\n";
    for (int i = 0; i <= c; i++)
    {
        makeStr(tmpo, P[i], P[0] == init ? false : true);
        if (output == NULL)
            printf("%s", tmpo);
        if (output != NULL)
            sprintf(output[i], "%s", tmpo);
    }
    return 0;
}
int main()
{
    char INITIAL[11];
    scanf("%[^\n]", INITIAL);
    fgetc(stdin);
    char TARGET[11];
    scanf("%[^\n]", TARGET);
    return (solution(NULL, makeByte(INITIAL), makeByte(TARGET)));
}