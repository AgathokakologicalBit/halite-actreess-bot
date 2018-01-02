#pragma once


#include "../../hlt/location.hpp"


struct Point : public hlt::Location
{
    unsigned int id;
    unsigned int score;

    Point ()
            : hlt::Location(0, 0)
              , id(0)
              , score(0)
    { }

    Point (unsigned int id, double x, double y)
            : hlt::Location(x, y)
              , id(id)
              , score(0)
    { }
};
