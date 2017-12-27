#pragma once


#include <cassert>
#include <vector>
#include <limits>
#include "../../hlt/location.hpp"

class Path
{
public:
    std::vector<hlt::Location> waypoints;
    std::vector<hlt::Location> original_graph;

    Path ()
            : waypoints()
    { }

public:
    void add (hlt::Location const location)
    {
        waypoints.push_back(location);
    }

    void insert (std::size_t index, hlt::Location const location)
    {
        assert(index < waypoints.size());
        waypoints.insert(waypoints.begin() + index, location);
    }
};
