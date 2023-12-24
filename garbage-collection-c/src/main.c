#include <stdio.h>

#define ATTR(...) __attribute__((__VA_ARGS__))
#define ADDMETHOD(name, ret, ...)\
    ret __##name(__VA_ARGS__) ATTR(const, alias(#name));\
    int func(int a, int b);\

ADDMETHOD(func, int, int, int);

void ATTR(constructor()) con()
{
    printf("constructor\n");
}

int main(void)
{
    {
        printf("func = %d", func(2, 3));
    }
    return 0;
}


int func(int a, int b)
{
    return a * b;
}
