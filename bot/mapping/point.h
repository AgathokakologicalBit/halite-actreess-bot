#pragma once


#include "../../hlt/location.hpp"


struct Point : public hlt::Location
{
public:
    unsigned int id;
    double score;


public:
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


public:
    bool operator < (Point & r) const { return score < r.score; }
};
