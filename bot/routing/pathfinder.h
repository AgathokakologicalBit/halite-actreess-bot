#pragma once

#include <vector>

#include "../hlt/hlt.hpp"
#include "path.h"

class Pathfinder
{
public:
    /**
     * Creates a list of all planets within a bounding box formed by two locations.
     * @param left_most_point   a point that *must* be on the left. (or else negatives :( )
     * @param right_most_point  a point that *must* be on the right. (or else negatives :( )
     * @return                  a list of all planets within bounding box
    */
    static std::vector<hlt::Planet *> get_in_bounds(hlt::Location * left_most_point, hlt::Location * right_most_point)
    {

    }

    /**
     * Creates a list of all planets intersecting a line from an origin to a goal
     * @param origin   the location to start the line at.
     * @param goal   the location to end the line at.
     * @return planets  the planets to check for intersection
    */
    static std::vector<hlt::Planet *> get_intersecting(hlt::Location * origin, hlt::Location * goal, std::vector<hlt::Planet *> planets)
    {

    }

    /**
     * Sorts a list of planets by their distance from an origin
     * @param 
    */
    static std::vector<hlt::Planet *> sort_by_distance(std::vector<hlt::Planet *> list, hlt::Location * origin)
    {

    }

    /**
     * Builds a graph of points for navigating around a provided list of obstacles from location origin to goal
     * @param origin   the starting location
     * @param goal   the ending location
     * @param planets  a list of planets to build points around
     * @return         a list of locations forming a graph around the obstacles with first point being origin and last being goal
    */
    static std::vector<hlt::Location *> build_graph(hlt::Location * origin, hlt::Location * goal, std::vector<hlt::Planet *> planets)
    {

    }

    /**
     * Finds the optimal path along a graph of points where the first point is the origin and the last point is the goal
     * @param path  the list of locations forming the graph where the first is the origin location and the last is the goal
     * @return      a path object
    */
    static Path * find_optimal_path(std::vector<hlt::Location *> path)
    {

    }
};
