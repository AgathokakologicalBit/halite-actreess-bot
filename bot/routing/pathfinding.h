#pragma once


#include <debug/gizmos.h>
#include "../mapping/imap.h"
#include "path.h"


class IMap;

class PathFinder
{
public:
    static Path find_path (Bot const & bot, Entity const & e, hlt::Location target, IMap const & map, double fl_distance)
    {
        Path path;

        auto const start = *map.get_point(static_cast<unsigned>(e.obj.location.x * map.get_width() / bot.map->width),
                                   static_cast<unsigned>(std::round(e.obj.location.y * map.get_height() / bot.map->height)));

        path.add(start);
        path.add(target);

        Gizmos::line(start, target);

        return path;
    }
};
