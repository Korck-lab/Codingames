#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
    bool mG = Fa == Ga ? (Wa < Ga && Ca < Ga) || (Wa > Ga && Ca > Ga) || (Wa == Ga && Ca == Ga): Ga;  
    bool mWi = (Wa == Fa && !Wa && mG == Ga) || Wa;
    bool mCi = (Ca == Fa && !Ca && mWi == Wa && mG == Ga) || Ca;
    bool mC = (Ca == Fa && !Ca && mG == Ga) || Ca;
    bool mW = (Wa == Fa && !Wa && mC == Ca && mG == Ga) || Wa;
    byte nextState = (F * mF) | (W * mW) | (G * mG) | (C * mC);
    byte nextStatei = (F * mF) | (W * mWi) | (G * mG) | (C * mCi);
    
    return invW?nextStatei:nextState;
}

void tests()
{
    char ints[][9] = {"L L L L", "R R R R", "L R L R", "L R L L", "R L R R"};
    byte tars[][9] = {"R L R L", "L R L R", "R R R R", "R R L R", "L L R R"};
    char b[9]= "? ? ? ? ";
    bool t = true;
    bool f = false;
    printf("%s - %s - %d\n",ints[0],makeStr(b, crossStep(makeByte(ints[0]), f), f), f);
    printf("%s - %s - %d\n",ints[0],makeStr(b, crossStep(makeByte(ints[0]), t), t), t);
    printf("%s - %s - %d\n",ints[1],makeStr(b, crossStep(makeByte(ints[1]), f), f), f);
    printf("%s - %s - %d\n",ints[1],makeStr(b, crossStep(makeByte(ints[1]), t), t), t);
    printf("%s - %s - %d\n",ints[2],makeStr(b, crossStep(makeByte(ints[2]), f), f), f);
    printf("%s - %s - %d\n",ints[2],makeStr(b, crossStep(makeByte(ints[2]), t), t), t);
    printf("%s - %s - %d\n",ints[3],makeStr(b, crossStep(makeByte(ints[3]), f), f), f);
    printf("%s - %s - %d\n",ints[3],makeStr(b, crossStep(makeByte(ints[3]), t), t), t);
    printf("%s - %s - %d\n",ints[4],makeStr(b, crossStep(makeByte(ints[4]), f), f), f);
    printf("%s - %s - %d\n",ints[4],makeStr(b, crossStep(makeByte(ints[4]), t), t), t);
}

int solution(byte init, byte target)
{
    if(init == target)
    {
        char b[9];
        printf(makeStr(b, init, false));
        return 0;
    }

    bool inv = ((init&W)+(init&C)) > ((target&W)+(target&C));
    if (inv) // XOR
    {
        init = init^0xF;
        target = target^0xF;
    }
    short maxP = 4 * 3 * 2 / (4 - 2);
    byte P0[maxP];
    byte P1[maxP];
    P0[0] = init;
    P1[0] = init;
    byte *P;
    short c=0;
    for (int i = 0; i < maxP - 1; i++)
    {
        P0[i + 1] = crossStep(P0[i], false^inv);
        P1[i + 1] = crossStep(P0[i], true^inv);
        c++;
        if (P0[i+1] == target)
        {
            P=P0;
            break;
        }
        if (P1[i+1] == target)
        {
            P=P1;
            break;
        }
    }
    char output[] = "? ? ? ?\n";
    for(int i=0; i<=c; i++)
    {
        printf(makeStr(output, P[i], inv));
    }
    return 0;
}

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
    tests();

    char INITIAL[11];
    scanf("%[^\n]", INITIAL);
    fgetc(stdin);
    char TARGET[11];
    scanf("%[^\n]", TARGET);
    byte init = makeByte(INITIAL);
    byte target = makeByte(TARGET);
    return(solution(init, target));
}