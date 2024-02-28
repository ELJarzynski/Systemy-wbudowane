#include <stdio.h>
#include <stdlib.h>

#define BIT_VALUE(val, no_bit) (val >> no_bit) & 1

void delay()
{
    for(int a=0; a<1000000; a++)
    {
        for(int b=0; b<1000; b++)
        {

        }
    }
}

void gen(int seed)
{
    int bit;

    while(seed != 0)
    {
        bit = BIT_VALUE(seed, 0) ^ BIT_VALUE(seed, 1) ^ BIT_VALUE(seed, 4) ^ BIT_VALUE(seed, 5);
        seed = seed >> 1 | bit << 5;
        printf("%d\n",seed);
        delay();
    }
}

void gen2(int seed)
{
    int bit, bit_2;

    while(seed != 0)
    {
        int seed_2;
        int result;
        bit = BIT_VALUE(seed, 0) ^ BIT_VALUE(seed, 1) ^ BIT_VALUE(seed, 4) ^ BIT_VALUE(seed, 5);
        bit_2 = BIT_VALUE(seed, 0) ^ BIT_VALUE(seed, 3) ^ BIT_VALUE(seed, 4) ^ BIT_VALUE(seed, 5);

        seed = seed >> 1 | bit << 5;
        seed_2 = seed >> 1 | bit_2 << 5;

        result = seed ^ seed_2;
        printf("%d\n",result);
        delay();
    }
}

int main()
{
    //gen(7);
    gen2(7);
    return 0;
}
