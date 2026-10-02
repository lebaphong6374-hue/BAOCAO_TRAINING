#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int *p = malloc(100 * sizeof(int));

    if (p == NULL)
    {
        printf("Cap phat bo nho that bai\n");
        return 1;
    }

    for (int i = 0; i < 100; i++)
    {
        p[i] = i;
    }

    printf("p[10] = %d\n", p[10]);

    free(p);
    p = NULL;

    return 0;
}