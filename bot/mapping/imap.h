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

    virtual Point * get_point (unsigned x, unsigned y) const = 0;

    virtual std::vector<Connection> get_connections (Point const *) const = 0;

    virtual unsigned get_size() const = 0;
};
