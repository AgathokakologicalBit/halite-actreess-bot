#pragma once

#include <cstddef>
#include <list>
#include <map>
#include <string>


#include "debug/gizmos.h"
#include "debug/timer.h"
#include "drone.h"
#include "routing/router.h"
#include "routing/pathfinder.h"
#include "../hlt/hlt.hpp"
#include "../hlt/navigation.hpp"


class Bot final
{
public:
    int id;

    std::vector<Drone *> drones;
    std::map<int, Drone *> my_drones;

    Router router;

    explicit Bot (hlt::Metadata data)
            : id(data.player_id)
    {
        Timer $timer_bot_init("bot init");

        srand(static_cast<unsigned>(time(nullptr)));
        drones.resize(10000);

        for (const auto & p : data.initial_map.ships)
        {
            for (const auto & s : p.second)
            {
                auto d = new Drone(s);

                this->drones[s.entity_id] = d;
                if (this->id == p.first)
                    this->my_drones[s.entity_id] = d;
            }
        }
    }

    std::vector<hlt::Move> make_turn (const hlt::Map & map)
    {
        using namespace hlt;

        Timer $timer_whole_turn("whole turn");

        for (auto d : this->drones)
        {
            if (!d) break;

            if (d->life_state < 2)
                d->life_state += 1;
        }

        for (const auto & p : map.ships)
        {
            for (const auto & s : p.second)
            {
                if (!this->drones[s.entity_id])
                {
                    this->drones[s.entity_id] = new Drone(s);
                    if (s.owner_id == this->id)
                        this->my_drones[s.entity_id] = this->drones[s.entity_id];

                    Log::log("Bot #" + std::to_string(s.entity_id) + " was spawned");
                }

                this->drones[s.entity_id]->update(s);
            }
        }

        for (auto d : this->drones)
        {
            if (!d) break;
            if (d->life_state != 1) continue;

            Log::log("Bot #" + std::to_string(d->id) + " was destroyed");
            if (this->my_drones.count(d->id))
                this->my_drones.erase(d->id);
        }

        std::vector<Move> moves;
        Log::log(std::string("Alive ships: ") + std::to_string(this->my_drones.size()));

        Timer $timer_strategies_evaluation("strategies evaluation");

        std::vector<const Ship *> swarm;
        for (auto drone : this->my_drones)
            swarm.push_back(&drone.second->ship);

        auto center = Router::get_fleet_center(swarm);

        static hlt::Planet target;
        static hlt::Location loc;
        static bool unreachable = true;
        static double fl_distance = 0;
        while (unreachable || target.location.get_distance_to(center) < 20)
        {
            target = map.planets[random() % map.planets.size()];
            Log::log("Target planet: " + std::to_string(target.entity_id));
            unreachable = false;

            loc = {
                    target.location.x,
                    target.location.y
            };
            fl_distance = target.radius + 5;
        }


        std::vector<Entity> entities;

        for (const auto & p : map.planets)
                entities.push_back(p);

        for (const auto & p : map.ships)
            if (p.first != this->id)
                for (auto s : p.second)
                    if (s.docking_status == hlt::ShipDockingStatus::Undocked)
                        entities.push_back((s.radius = hlt::constants::WEAPON_RADIUS + hlt::constants::MAX_SPEED, s));

        auto path = PathFinder::find_path(center, loc, entities, 4, fl_distance);
        if (path.waypoints.empty())
        {
            router.move(moves, swarm, center);
            unreachable = true;
        }
        else
        {
            router.move(moves, swarm, path.waypoints[1]);
        }

        return moves;
    }
};
