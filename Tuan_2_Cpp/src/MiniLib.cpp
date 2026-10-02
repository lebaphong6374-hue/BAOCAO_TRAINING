#include "MiniLib.h"

namespace mini
{
    int add(int a, int b)
    {
        return a + b;
    }

    int subtract(int a, int b)
    {
        return a - b;
    }

    int multiply(int a, int b)
    {
        return a * b;
    }

    bool isEven(int n)
    {
        return n % 2 == 0;
    }
}