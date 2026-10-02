#include <iostream>
#include <string>

#include "MiniLib.h"
#include "Stack.h"

int main()
{
    std::cout << "===== MINI LIBRARY =====\n";

    int a = 10;
    int b = 5;

    std::cout << "add = " << mini::add(a, b) << '\n';
    std::cout << "subtract = " << mini::subtract(a, b) << '\n';
    std::cout << "multiply = " << mini::multiply(a, b) << '\n';

    if (mini::isEven(a))
    {
        std::cout << a << " la so chan\n";
    }
    else
    {
        std::cout << a << " la so le\n";
    }

    std::cout << "\n===== STACK<int> =====\n";

    Stack<int> intStack;

    intStack.push(10);
    intStack.push(20);
    intStack.push(30);

    std::cout << "Top = " << intStack.top() << '\n';
    std::cout << "Size = " << intStack.size() << '\n';

    intStack.pop();

    std::cout << "Sau pop:\n";
    std::cout << "Top = " << intStack.top() << '\n';
    std::cout << "Size = " << intStack.size() << '\n';

    std::cout << "\n===== STACK<string> =====\n";

    Stack<std::string> stringStack;

    stringStack.push("Tuan");
    stringStack.push("Embedded");
    stringStack.push("Linux");

    std::cout << "Top = " << stringStack.top() << '\n';

    stringStack.pop();

    std::cout << "Sau pop, Top = " << stringStack.top() << '\n';

    return 0;
}