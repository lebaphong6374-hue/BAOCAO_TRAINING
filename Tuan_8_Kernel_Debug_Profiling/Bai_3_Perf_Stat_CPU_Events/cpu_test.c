#include <stdio.h>

int main(void)
{
    volatile unsigned long long sum = 0;
    unsigned long long i;

    for (i = 0; i < 200000000ULL; i++)
    {
        sum += i;
    }

    printf("sum = %llu\n", sum);

    return 0;
}
