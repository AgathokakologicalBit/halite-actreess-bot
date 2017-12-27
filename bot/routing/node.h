#pragma once


#include <utility>
#include <vector>
#include <string>

class Node
{

public:
    std::string id;

    double x;
    double y;

    std::vector<std::string> connected;

    Node (std::string id, double x, double y)
            : id(std::move(id))
            , x(x)
            , y(y)
    { }

    void connect (std::string const & id)
    {
        connected.push_back(id);
    }
};
