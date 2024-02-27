#include <stdio.h>
#include <stdlib.h>

void toBit(int n)
{
    int counter[8];
    int i = 0;
    while(i < 8)
    {
        counter[i] = n & 1;
        n = n >> 1;
        i++;
    }

    for(int j = i-1; j>=0; j--)
    {
        printf("%d", counter[j]);
    }
}

void toGray(int n)
{
    int counter[8];
    int i = 0;
    int msb = 0; // Initialize most significant bit

    while (n > 0)
    {
        counter[i++] = n & 1;
        n >>= 1;
    }

    for (int j = 0; j < i; j++)
    {
        counter[j] ^= msb; // XOR with the most significant bit
        msb = counter[j]; // Update most significant bit
    }

    for (int j = i - 1; j >= 0; j--)
    {
        printf("%d", counter[j]);
    }
    printf("\n");
}

void delay()
{
    for(int a=0; a<1000000; a++)
    {
        for(int b=0; b<1000; b++)
        {

        }
    }
}


int main()
{
    toGray(7);
}
