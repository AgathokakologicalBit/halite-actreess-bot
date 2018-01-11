#pragma once


#include "connection.h"
#include "imap.h"
#include "../bot.h"

#include <algorithm>
#include <map>
#include <vector>
#include <memory>
#include <debug/gizmos.h>


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


private:
    template <typename $ET>
    void map (Bot const & bot, std::string name, std::map<int, $ET> entities, float targets_radius, float multiplier)
    {
        Timer $timer_analyze_map("entities analysis " + name);

        // calculating new scores
        double const wmult = static_cast<double>(width) / bot.map->width;

        for (auto const & e : entities)
        {
            std::set<std::pair<unsigned, unsigned>> targets;

            const auto & obj = e.second->obj;
            auto radius = targets_radius + obj.radius;

            int sx = std::max(static_cast<int>((obj.location.x - radius) * wmult - 1), 0);
            int sy = std::max(static_cast<int>((obj.location.y - radius) * wmult - 1), 0);

            int ex = std::min(static_cast<int>((obj.location.x + radius) * wmult) + 2,
                              static_cast<int>(width - 1));
            int ey = std::min(static_cast<int>((obj.location.y + radius) * wmult) + 2,
                              static_cast<int>(height - 1));

            for (int y = sy; y < ey; ++y)
            {
                for (int x = sx; x < ex; ++x)
                {
                    auto & p = points[y * width + x];

                    auto dst = obj.location.get_distance_to(p);
                    if (dst > radius)
                        continue;

                    p.score += (1 + obj.health / 100) * multiplier;
                    p.score = std::abs(p.score);
                    Gizmos::set_color(Gizmos::color_from_rgb(50, 150, 255));
                    Gizmos::line(obj.location, p);
                }
            }
        }
    }

public:
    void analyze (Bot const & bot) final
    {
        Timer $timer_analyze_map("map analysis");
        connections.clear();

        // resetting scores
        for (std::size_t i = 0; i < this->width * this->height; ++i)
            points[i].score = 0;

        map(bot, "planets", bot.planets, 5, 1);
        map(bot, "drones", bot.drones, 5, 1);
        map(bot, "drones-attack", bot.drones, 5 + static_cast<float>(hlt::constants::WEAPON_RADIUS), .5);
        map(bot, "player-attack", bot.my_drones, 5 + static_cast<float>(hlt::constants::WEAPON_RADIUS), -1);
    }

    Point * get_point (unsigned x, unsigned y) final
    {
        return points + (y * this->width + x);
    }

    std::vector<Connection> get_connections (Point const * p) final
    {
        std::vector<Connection> connections;
        hlt::Location positions[]{
                { +1, 0 },
                { 0,  +1 },
                { -1, 0 },
                { 0,  -1 }
        };

        for (auto s : positions)
        {
            if (p->x + s.x < 0 || p->x + s.x >= width) continue;
            if (p->y + s.y < 0 || p->y + s.y >= height) continue;

            auto e = &this->points[p->id + static_cast<int>(s.y) * width + static_cast<int>(s.x)];
            connections.push_back({ p, e, (p->score + e->score) * p->get_distance_to(*e) });
        }

        return connections;
    }
};
