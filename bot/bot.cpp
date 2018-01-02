#include "bot.h"
#include "routing/pathfinding.h"
#include "mapping/force_map.h"
#include "debug/gizmos.h"


Bot::Bot (hlt::Metadata data)
        : id(data.player_id)
          , map(nullptr)
{
    Timer $timer_bot_init("bot init");

    this->map = &data.initial_map;

    this->navmap_force = new ForceMap(*this, data.initial_map.width / 10, data.initial_map.height / 10);

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

std::vector<hlt::Move> Bot::make_turn (const hlt::Map & map)
{
    using hlt::Log;
    Timer $timer_whole_turn("whole turn");

    this->map = &map;

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

    std::vector<hlt::Move> moves;
    Log::log(std::string("Alive ships: ") + std::to_string(this->my_drones.size()));

    Timer $timer_strategies_evaluation("strategies evaluation");

    std::vector<const hlt::Ship *> swarm;
    for (auto drone : this->my_drones)
        swarm.push_back(&drone.second->ship);

    auto center = Router::get_fleet_center(swarm);

    static hlt::Location target;
    static double fl_distance = 20;
    while (target.get_distance_to(center) < fl_distance)
    {
        target.x = random() / static_cast<double>(map.width);
        target.y = random() / static_cast<double>(map.height);
    }


    std::vector<hlt::Entity> entities;
    for (const auto & p : map.planets)
        entities.push_back(p);

    for (const auto & p : map.ships)
        if (p.first != this->id)
            for (auto s : p.second)
                if (s.docking_status == hlt::ShipDockingStatus::Undocked)
                    entities.push_back((s.radius = hlt::constants::WEAPON_RADIUS + hlt::constants::MAX_SPEED, s));

    this->navmap_force->analyze(*this);

    for (unsigned y = 0; y < this->navmap_force->height; ++y)
    {
        for (unsigned x = 0; x < this->navmap_force->width; ++x)
        {
            auto s = *this->navmap_force->get_point(x, y);
            Gizmos::line(Point(0, s.x - 1, s.y + 1), Point(0, s.x + 1, s.y - 1));
        }
    }

    auto path = PathFinder::find_path(Entity(center, 4), target, *this->navmap_force, fl_distance);
    if (path.waypoints.empty())
    {
        router.move(moves, swarm, center);
    }
    else
    {
        router.move(moves, swarm, path.waypoints[1]);
    }

    return moves;
}
