#include <stdio.h>

int add(int x, int y)
{
    return x + y;
}
void exe(int (*operation)(int, int))
{
    // int res = operation[0](4, 3);
    int res = operation(7, 3);
    printf("res => %d", res);
}

struct structure
{
    int x;
    int y;
};
typedef structure IStruct;
void main()
{
    IStruct structure.x = x;
    IStruct structure.y = y;

    printf("structure contains %d and %d", structure.x, structure.y);
    exe(add);
}