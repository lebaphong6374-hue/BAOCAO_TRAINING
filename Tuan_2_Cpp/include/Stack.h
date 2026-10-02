#ifndef STACK_H
#define STACK_H

#include <vector>
#include <stdexcept>
#include <cstddef>

template <typename T>
class Stack
{
private:
    std::vector<T> data;

public:
    void push(const T& value)
    {
        data.push_back(value);
    }

    void pop()
    {
        if (data.empty())
        {
            throw std::runtime_error("Stack is empty");
        }

        data.pop_back();
    }

    T& top()
    {
        if (data.empty())
        {
            throw std::runtime_error("Stack is empty");
        }

        return data.back();
    }

    bool empty() const
    {
        return data.empty();
    }

    std::size_t size() const
    {
        return data.size();
    }

    void clear()
    {
        data.clear();
    }
};

#endif