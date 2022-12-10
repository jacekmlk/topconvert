#include <stdio.h>
#include <stdlib.h>

typedef struct node
{
    int a;
    int b;
}node;

void operate (int *alfa)
{
    printf("a[0] = %i, a[1] = %i\n", alfa[0], alfa[1]);
}

int main(void)
{
    int amount = 2;
    node alfa[amount];


    alfa[0].a = 1;
    alfa[1].a = 2;

    alfa[0].b = 4;
    alfa[1].b = 5;

    for(int i = 0; i < amount; i++)
    {
        printf("a = %i\n", alfa[i].a);
        printf("b = %i\n", alfa[i].b);
    }

    operate(&alfa->a);

}

