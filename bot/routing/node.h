#pragma once

#include <vector>
#include <string>

class Node
{

public:
    std::string id;

    double x;
    double y;

    std::vector<std::string> connected;

    Node(std::string id, double x, double y)
    {
        this->id = id;
        this->x = x;
        this->y = y;
    }

    void connect(std::string id)
    {
        connected.push_back(id);
    }
};
