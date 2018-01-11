#pragma once


#include "../debug/gizmos.h"
#include "../mapping/imap.h"
#include "path.h"

#include <queue>


class IMap;

class PathFinder
{
private:
    class HeuristicsComparator
    {
    public:
        hlt::Location target;

    public:
        HeuristicsComparator (hlt::Location target_)
                : target(target_)
        { }

        bool operator () (Connection const & a, Connection const & b) const
        {
            return a.score + a.end->get_distance_to(target) > b.score + b.end->get_distance_to(target);
        }
    };


private:
    static void
    move_back (Path * path, std::map<Point const *, Connection> visited, Point const * current, Point const * end,
               double fl_distance)
    {
        if (current->get_distance_to(*end) <= fl_distance)
            move_back(path, visited, visited[current].begin, end, fl_distance);

        path->add(*current);
    }

public:
    static Path
    find_path (Bot const & bot, Entity const & e, hlt::Location target, IMap const & map, double fl_distance)
    {
        Timer $timer_find_path("path finding");
        Path path;

        auto * const start = map.get_point(static_cast<unsigned>(e.obj.location.x * map.get_width() / bot.map->width),
                                           static_cast<unsigned>(std::round(
                                                   e.obj.location.y * map.get_height() / bot.map->height)));

        HeuristicsComparator comparator(target);
        std::priority_queue<Connection, std::vector<Connection>, HeuristicsComparator> targets(comparator);

        for (Connection c : map.get_connections(start))
            targets.push(c);

        std::map<Point const *, Connection> visited;

        while (!targets.empty() && targets.top().begin->get_distance_to(target) > fl_distance)
        {
            auto next = targets.top();
            targets.pop();

            for (auto c : map.get_connections(next.end))
                if (c.end != next.begin)
                    targets.push(c);

            if (visited[next.end].score > next.score)
                visited[next.end] = next;

            Gizmos::line(next.begin, next.end);
        }

        Point const * end = targets.empty() ? nullptr : targets.top().begin;
        if (!end) return path;

        move_back(&path, visited, end, start, fl_distance);

        Gizmos::line(*start, target);

        return path;
    }
};
