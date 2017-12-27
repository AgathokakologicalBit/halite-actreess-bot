#pragma once

#include <vector>
#include <algorithm>
#include <cmath>
#include <map>

#include "../../hlt/log.hpp"
#include "../../hlt/hlt.hpp"
#include "path.h"
#include "node.h"

class PathFinder
{
public:
    /**
     * Creates a list of all planets within a bounding box formed by two locations.
     * @param left_most_point   a point that *must* be on the left. (or else negatives :( )
     * @param right_most_point  a point that *must* be on the right. (or else negatives :( )
     * @return                  a list of all planets within bounding box
    */
    static std::vector<const hlt::Planet *>
    get_in_bounds (hlt::Location left_most_point, hlt::Location right_most_point, const hlt::Map & map)
    {
        std::vector<const hlt::Planet *> planets;

        // create references to positions for convenience and clarity
        auto const min_x = left_most_point.pos_x;
        auto const min_y = left_most_point.pos_y;
        auto const max_x = right_most_point.pos_x;
        auto const max_y = right_most_point.pos_y;

        // iterate over planets adding the ones that are in bounds
        for (auto & planet : map.planets)
        {
            if (planet.location.pos_x >= min_x && planet.location.pos_x <= max_x)
            {
                if (planet.location.pos_y >= min_y && planet.location.pos_y <= max_y)
                {
                    planets.push_back(&planet);
                }
            }
        }

        return planets;
    }

    /**
     * Creates a list of all planets intersecting a line from an origin to a goal
     * @param origin    the location to start the line at.
     * @param goal      the location to end the line at.
     * @return planets  the planets to check for intersection
    */
    static std::vector<hlt::Planet>
    get_intersecting (hlt::Location origin, hlt::Location goal, std::vector<hlt::Planet> const & planets)
    {
        std::vector<hlt::Planet> intersecting_planets;

        double distance_from_line;

        for (auto & planet : planets)
        {
            distance_from_line = point_distance_from_line(
                    origin.pos_x,
                    origin.pos_y,
                    goal.pos_x,
                    goal.pos_y,
                    planet.location.pos_x,
                    planet.location.pos_y
            );

            // if the distance from the planet center to the line is >= the planet's radius
            // then we add it to the intersection vector
            if (distance_from_line <= planet.radius)
            {
                intersecting_planets.push_back(planet);
            }
        }

        return intersecting_planets;
    }

    /**
     * Computes the distance a point is from a line
     * Adapted from: https://math.stackexchange.com/questions/275529/check-if-line-intersects-with-circles-perimeter
     * @param line_start_x  the x of the starting point of the line
     * @param line_start_y  the y of the starting point of the line
     * @param line_end_x    the x of the ending point of the line
     * @param line_end_y    the y of the ending point of the line
     * @param point_x       the x of the point to measure from
     * @param point_y       the y of the point to measure from
     * @return              the distance from the point to the closest point on the line
    */
    static double point_distance_from_line (
            const double line_start_x,
            const double line_start_y,
            const double line_end_x,
            const double line_end_y,
            const double point_x,
            const double point_y
    )
    {
        return std::abs(
                (line_end_y - line_start_y) * point_x +
                (line_start_x - line_end_x) * point_y +
                (line_start_y - line_end_y) * point_x +
                (line_start_x - line_end_x) * point_y
        ) / std::sqrt(
                std::pow((line_end_y - line_start_y), 2.0) +
                std::pow((line_start_x - line_end_x), 2.0)
        );
    }

    /**
     * Sorts a list of planets by their distance from an origin
     * @param list    list of planets to sort
     * @param origin  origin point to sort planets from
    */
    static std::vector<hlt::Planet> sort_by_distance (std::vector<hlt::Planet> planets, hlt::Location origin)
    {
        // use a custom sorter to sort planets from closest to farthest
        std::sort(planets.begin(), planets.end(), distance_sorter(origin));
        return planets;
    }

    /**
     * Builds a graph of points for navigating around a provided list of obstacles from location origin to goal
     * @param origin   the starting location
     * @param goal     the ending location
     * @param planets  a list of planets to build points around
     * @param padding  the padding to place between the graph nodes and the planets (0 would be right on the surface)
     * @return         a list of locations forming a graph around the obstacles with first point being origin and last being goal
    */
    static std::map<std::string, Node *>
    build_graph (hlt::Location origin, hlt::Location goal, std::vector<hlt::Planet> const & planets, const double padding)
    {
        Timer $timer_build_graph("build search graph");

        std::map<std::string, Node *> graph;

        // add origin to the graph
        graph.insert(std::pair<std::string, Node *>("start", new Node(
                "start",
                origin.pos_x,
                origin.pos_y
        )));

        std::vector<std::string> previous_nodes;
        previous_nodes.reserve(planets.size());

        // iterate through the planets from closest to farthest computing their edge points and connecting
        // the nodes to form a graph
        for (std::size_t i = 0; i < planets.size(); i++)
        {
            std::vector<Node *> edge_points = get_edge_points(origin, planets[i], padding);

            // the first planet process will be connected to our start node
            if (i == 0)
            {
                for (Node * edge_point : edge_points)
                {
                    hlt::Log::log(
                            "[Line] " +
                            std::to_string(edge_point->x) + ' ' +
                            std::to_string(edge_point->y) + ' ' +
                            std::to_string(graph["start"]->x) + ' ' +
                            std::to_string(graph["start"]->y)
                    );

                    edge_point->connect("start");
                    previous_nodes.push_back(edge_point->id);
                    graph[edge_point->id] = edge_point;
                }
            }
                // every sequential planet is connected to the previous two planets' edge nodes
            else
            {
                for (Node * edge_point : edge_points)
                {
                    hlt::Log::log(
                            "[Line] " +
                            std::to_string(edge_point->x) + ' ' +
                            std::to_string(edge_point->y) + ' ' +
                            std::to_string(graph[previous_nodes[previous_nodes.size() - 1]]->x) + ' ' +
                            std::to_string(graph[previous_nodes[previous_nodes.size() - 1]]->y)
                    );
                    hlt::Log::log(
                            "[Line] " +
                            std::to_string(edge_point->x) + ' ' +
                            std::to_string(edge_point->y) + ' ' +
                            std::to_string(graph[previous_nodes[previous_nodes.size() - 2]]->x) + ' ' +
                            std::to_string(graph[previous_nodes[previous_nodes.size() - 2]]->y)
                    );
                    edge_point->connect(previous_nodes[previous_nodes.size() - 1]);
                    edge_point->connect(previous_nodes[previous_nodes.size() - 2]);
                    graph[edge_point->id] = edge_point;
                }

                for (Node * edge_point : edge_points)
                {
                    previous_nodes.push_back(edge_point->id);
                }
            }
        }

        // add the goal node
        Node * goalNode = new Node(
                "end",
                goal.pos_x,
                goal.pos_y
        );

        // connect the goal node to the previous two planets' edge nodes
        goalNode->connect(previous_nodes[previous_nodes.size() - 1]);
        goalNode->connect(previous_nodes[previous_nodes.size() - 2]);
        graph.insert(std::pair<std::string, Node *>("end", goalNode));

        return graph;
    }

    /**
     * Get's the edge points around the circumference of a sphere
     * @param origin   the origin location to calculate from
     * @param planet   the planet to compute edge points against
     * @param padding  how far the edge points should be from the planet surface
     * @return         a pair of nodes; the edge points on the sphere perpendicular to the line from the origin to the planet
    */
    static std::vector<Node *> get_edge_points (hlt::Location origin, hlt::Planet const & planet, const double padding)
    {
        std::vector<Node *> edge_points;
        edge_points.reserve(2);

        const double angle_from_horizon = origin.orient_towards_in_rad(planet.location);
        const double distance_to_center = origin.get_distance_to(planet.location);
        // we use arctan instead of arctan2 beacuse we only want the small angle
        const double tangent_angle = atan((planet.radius + padding) / distance_to_center);
        const double tangent_length = distance_to_center / cos(tangent_angle);
        const double node_x = tangent_length * std::sin(tangent_angle + angle_from_horizon);
        const double node_y = tangent_length * std::cos(tangent_angle + angle_from_horizon);
        const double opposite_node_x = planet.location.pos_x + (node_x - planet.location.pos_x);
        const double opposite_node_y = planet.location.pos_y + (node_y - planet.location.pos_y);

        // add the edge points to the vector
        // id has either a or b prepended: a for above, b for below
        edge_points.push_back(new Node(std::to_string(planet.entity_id) + "a", node_x, node_y));
        edge_points.push_back(new Node(std::to_string(planet.entity_id) + "b", opposite_node_x, opposite_node_y));

        return edge_points;
    }

    /**
     * Finds the optimal path along a graph of points where the first point is the origin and the last point is the goal
     * @param path  the list of locations forming the graph where the first is the origin location and the last is the goal
     * @return      a path object
    */
    static Path * find_optimal_path (std::vector<hlt::Location *> & path)
    {
        return nullptr;
    }

private:
    /**
     * Object used by std::sort to sort planet's based on their distance from an origin point
    */
    struct distance_sorter final
    {
        hlt::Location origin;

        explicit distance_sorter (hlt::Location origin)
                : origin(origin)
        { }

        /**
         * Function that computes if a planet is closer or farther to the origin than another (used by std::sort)
        */
        bool operator () (hlt::Planet const & first, hlt::Planet const & second) const
        {
            return first.location.get_distance_to(this->origin) < second.location.get_distance_to(this->origin);
        }
    };
};
