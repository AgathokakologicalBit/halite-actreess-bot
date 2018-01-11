#pragma once


#include <cassert>
#include <vector>
#include <limits>
#include "../../hlt/location.hpp"

class Path
{
public:
    std::vector<hlt::Location> waypoints;
    std::size_t length;

    Path ()
            : waypoints()
            , length(0)
    { }

public:
    void add (hlt::Location const location)
    {
        if (!waypoints.empty())
            length += waypoints.front().get_distance_to(location);
        waypoints.push_back(location);
    }

    void insert (std::size_t index, hlt::Location const location)
    {
        assert(index < waypoints.size());
        if (index == waypoints.size() - 1)
        {
            if (!waypoints.empty())
                length += waypoints.front().get_distance_to(location);
            waypoints.push_back(location);
            return;
        }

        length -= waypoints[index].get_distance_to(waypoints[index + 1]);
        length += waypoints[index].get_distance_to(location);
        length += waypoints[index + 1].get_distance_to(location);
        waypoints.insert(waypoints.begin() + index, location);
    }
};
