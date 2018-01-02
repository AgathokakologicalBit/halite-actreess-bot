#pragma once


#include "connection.h"
#include "imap.h"
#include "../bot.h"

#include <map>
#include <vector>
#include <memory>


class ForceMap final : public IMap
{
public:
    unsigned width, height;

    Point * points;
    std::map<unsigned, std::vector<Connection>> connections;


    ForceMap (Bot const & bot, unsigned width, unsigned height)
            : width(width)
              , height(height)
              , points(new Point[width * height])
    {
        for (unsigned y = 0; y < this->height; ++y)
        {
            for (unsigned x = 0; x < this->width; ++x)
            {
                double mx = bot.map->width;
                double my = bot.map->height;

                double hx = bot.map->width / width / 2;
                double hy = bot.map->height / height / 2;

                unsigned id = y * width + x;
                points[id] = Point(id, x * mx / width + hx, y * my / height + hy);
            }
        }
    }

    ~ForceMap () = default;


    void analyze (Bot const & bot) final
    {
        Timer $timer_analyze_map("map analysis");
        connections.clear();

        // TODO: Analyze map;
    }

    Point * get_point (unsigned x, unsigned y) final
    {
        return points + (y * this->width + x);
    }

    std::vector<Connection> get_connections (Point const *) final
    {
        return {};
    }
};
