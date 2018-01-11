#pragma once


#include "../mapping/imap.h"
#include "path.h"


class IMap;

class PathFinder
{
public:
    static Path find_path (Entity const & e, hlt::Location t, IMap const & map, double fl_distance)
    {
        Path path;
        return path;
    }
};
