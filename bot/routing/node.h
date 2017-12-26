#pragma once

#include <vector>

class Node
{
public:
    int id;

    double x;
    double y;

    std::vector<int> connected;

    Node(int id, double x, double y)
    {
        this->id = id;
        this->x = x;
        this->y = y;
    }
};
