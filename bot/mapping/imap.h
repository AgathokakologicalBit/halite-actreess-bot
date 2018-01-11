#pragma once


#include "../../hlt/map.hpp"
#include "point.h"
#include "connection.h"
#include "../bot.h"

class Bot;

class IMap
{
public:
    virtual void analyze (Bot const &) = 0;

    virtual Point * get_point (unsigned x, unsigned y) = 0;

    virtual std::vector<Connection> get_connections (Point const *) = 0;
};
