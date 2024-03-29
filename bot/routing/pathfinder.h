#pragma once

#include <vector>
#include <algorithm>
#include <cmath>
#include <map>

#include "../../hlt/log.hpp"
#include "../../hlt/hlt.hpp"
#include "path.h"
#include "node.h"

class $old$PathFinder
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
        auto const min_x = left_most_point.x;
        auto const min_y = left_most_point.y;
        auto const max_x = right_most_point.x;
        auto const max_y = right_most_point.y;

        // iterate over planets adding the ones that are in bounds
        for (auto & planet : map.planets)
        {
            if (planet.location.x >= min_x && planet.location.x <= max_x)
            {
                if (planet.location.y >= min_y && planet.location.y <= max_y)
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
     * @param end      the location to end the line at.
     * @return planets  the planets to check for intersection
    */
    static std::vector<hlt::Entity>
    get_intersecting
            (
                    hlt::Location goal,
                    hlt::Location origin,
                    hlt::Location end,
                    std::vector<hlt::Entity> const & entities,
                    double radius,
                    double fl_distance
            )
    {
        std::vector<hlt::Entity> intersecting_entities;

        for (auto & entity : entities)
        {
            double dfl = point_distance_from_line(
                    origin,
                    end,
                    entity.location
            );

            if (std::abs(dfl) <= entity.radius + radius
                && origin.get_distance_to(goal) - origin.get_distance_to(entity.location) - fl_distance + entity.radius + radius > 0
                && origin.get_distance_to(end) + entity.radius + radius > entity.location.get_distance_to(end)
                && origin.get_distance_to(end) + entity.radius + radius > origin.get_distance_to(entity.location))
            {
                intersecting_entities.push_back(entity);
            }
        }

        return intersecting_entities;
    }

    static double point_distance_from_line (
            hlt::Location start,
            hlt::Location end,
            hlt::Location point
    )
    {
        hlt::Location line{
                end.x - start.x,
                end.y - start.y,
        }; // direction of the line
        auto len = line.get_distance_to({ 0, 0 });
        line.x /= len;
        line.y /= len;

        hlt::Location point_to_start{
                start.x - point.x,
                start.y - point.y,
        }; // vector from the point to the start of the line

        auto dot = point_to_start.x * line.x + point_to_start.y * line.y; //dot product of point_to_start * line
        auto d = point_to_start.y * line.x - point_to_start.x * line.y;

        hlt::Location distance{
                point_to_start.x - dot * line.x,
                point_to_start.y - dot * line.y,
        }; //shortest distance vector from point to line

        return sgn(d) * sqrt(distance.x * distance.x + distance.y * distance.y);
    }

    /**
     * Sorts a list of planets by their distance from an origin
     * @param list    list of planets to sort
     * @param origin  origin point to sort planets from
    */
    static std::vector<hlt::Entity> sort_by_distance (std::vector<hlt::Entity> entities, hlt::Location origin)
    {
        // use a custom sorter to sort planets from closest to farthest
        std::sort(entities.begin(), entities.end(), distance_sorter(origin));
        return entities;
    }

private:
    template <typename T>
    static int sgn (T val)
    {
        return (T(0) <= val) - (val < T(0));
    }

public:
    /**
     * Finds the optimal path along a graph of points where the first point is the origin and the last point is the goal
     * @param path  the list of locations forming the graph where the first is the origin location and the last is the goal
     * @return      a path object
    */
    static Path find_path
            (
                    hlt::Location start,
                    hlt::Location end,
                    std::vector<hlt::Entity> const & entities,
                    double radius,
                    double fl_distance
            )
    {
        Path path;

        Gizmos::set_color(Gizmos::color_from_rgb(64, 64, 64));
        Gizmos::line(start, end);
        for (auto v : find_path(end, start, end, entities, radius, 2, fl_distance, -1, 0).waypoints)
            path.add(v);

        hlt::Log::log("Path size: " + std::to_string(path.waypoints.size()));
        if (path.waypoints.empty())
            return path;

        path.add(end);
        return path;
    }

private:
    static Path find_sub_path
            (
                    hlt::Location goal,
                    hlt::Location start,
                    hlt::Location end,
                    std::vector<hlt::Entity> const & entities,
                    double radius,
                    int depth,
                    double fl_distance,
                    unsigned int last_entity,
                    unsigned int count
            )
    {
        auto p = find_path(goal, start, end, entities, radius, depth + 1, fl_distance, last_entity, count);

        if (!p.waypoints.empty())
        {
            std::uint8_t const tint = 32;
            auto const col = Gizmos::color_from_rgb(tint, tint, tint);
            Gizmos::set_color(col * (depth + 1));
            Gizmos::line(start, end);
            Gizmos::set_color(0xFFFFFF);
        }

        return p;
    }

    static Path find_split_path
            (
                    hlt::Location goal,
                    hlt::Location start,
                    hlt::Location end,
                    std::vector<hlt::Entity> const & entities,
                    hlt::Entity const & intersect,
                    double nx, double ny,
                    double radius,
                    int depth,
                    double fl_distance,
                    unsigned int last_entity,
                    unsigned int count
            )
    {
        hlt::Location mid{
                intersect.location.x + nx,
                intersect.location.y + ny
        };

        for (auto & e : entities)
            if (e.radius + radius > mid.get_distance_to(e.location))
                return {};

        Path path;
        bool success = true;
        auto sub_path = find_sub_path(
                goal, start, mid, entities,
                radius, depth, fl_distance,
                intersect.entity_id,
                (intersect.entity_id == last_entity ? count : 0) + 1
        );
        for (auto v : sub_path.waypoints)
            path.add(v);

        if (success &= !sub_path.waypoints.empty())
        {
            sub_path = find_sub_path(
                    goal, mid, end, entities,
                    radius, depth, fl_distance,
                    intersect.entity_id,
                    (intersect.entity_id == last_entity ? count : 0) + 1
            );
            for (auto v : sub_path.waypoints)
                path.add(v);
            success &= !sub_path.waypoints.empty();
        }

        if (!success) path.waypoints.clear();
        return path;
    }

    static Path find_path (
            hlt::Location goal,
            hlt::Location start,
            hlt::Location end,
            std::vector<hlt::Entity> const & entities,
            double radius,
            int depth,
            double fl_distance,
            unsigned int last_entity,
            unsigned int count
    )
    {
        Path path;

        if (goal.get_distance_to(start) <= fl_distance)
        {
            for (auto & e : entities)
                if (e.radius + radius > start.get_distance_to(e.location))
                    return {};

            path.add(start);
            return path;
        }

        if (depth > 15)
            return { };

        auto intersects = get_intersecting(goal, start, end, entities, radius, fl_distance);

        while (!intersects.empty() && path.waypoints.empty())
        {
            auto intersect = sort_by_distance(intersects, start)[0];
            if (intersect.entity_id == last_entity && count > 3)
                return { };

            hlt::Location sub{ end.x - start.x, end.y - start.y };
            double len = sub.get_distance_to({ 0, 0 });
            hlt::Location normal{ -sub.y / len, sub.x / len };

            auto dst_ts = intersect.location.get_distance_to(start);
            auto nx = normal.x * (radius + intersect.radius * (dst_ts + 1) / dst_ts + 1);
            auto ny = normal.y * (radius + intersect.radius * (dst_ts + 1) / dst_ts + 1);

            path = find_split_path(
                    goal, start, end,
                    entities, intersect,
                    nx, ny,
                    radius, depth, fl_distance,
                    last_entity, count
            );
            auto np = find_split_path(
                    goal, start, end,
                    entities, intersect,
                    -nx, -ny,
                    radius, depth, fl_distance,
                    last_entity, count
            );

            if (path.waypoints.empty() || (!np.waypoints.empty() && np.length < path.length))
                path = np;

            intersects.erase(intersects.begin());
        }


        path.add(start);
        return path;
    };

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
        bool operator () (hlt::Entity const & first, hlt::Entity const & second) const
        {
            return first.location.get_distance_to(this->origin) < second.location.get_distance_to(this->origin);
        }
    };
};
