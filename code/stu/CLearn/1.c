#include <stdio.h>

int init(void)
{
    printf("EOF = %d\n", EOF);
    return 0;
}

int main(void)
{
    init();
    char i;

    i = getchar();

    while ((i = getchar()) != EOF)
    {
        if (i == 'q')
            return 0;
        printf("\n%d\t-->\t", i);
        putchar(i);
    }
    return 0;
}