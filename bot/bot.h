#pragma once

#include <cstddef>
#include <list>
#include <map>
#include <string>

#include "drone.h"
#include "router.h"
#include "../hlt/hlt.hpp"
#include "../hlt/navigation.hpp"


class Bot final {
public:
    int id;

    std::vector<Drone*> drones;
    std::map<int, Drone*> my_drones;

	Router *router = new Router();

    explicit Bot(hlt::Metadata data)
            : id(data.player_id)
    {
        drones.resize(1000);

        for (const auto& p : data.initial_map.ships) {
            for (const auto &s : p.second) {
                Drone *d = new Drone(s);

                this->drones.push_back(d);
                if (this->id == p.first)
                    this->my_drones[s.entity_id] = d;
            }
        }
    }

    std::vector<hlt::Move> make_turn(const hlt::Map& map) {
        using namespace hlt;

        for (auto& d : this->drones)
            if (d) d->life_state += 1;

        for (const auto& p : map.ships) {
			for (const auto &s : p.second) {
                if (!this->drones[s.entity_id]) {
                    this->drones[s.entity_id] = new Drone(s);
                    if (s.owner_id == this->id)
                        this->my_drones[s.entity_id] = this->drones[s.entity_id];

                    Log::log("Bot #" + std::to_string(s.entity_id) + " was spawned");
                }

                this->drones[s.entity_id]->update(s);
            }
        }

        for (auto& d : this->drones)
            if (d && d->life_state == 1)
                Log::log("Bot #" + std::to_string(d->id) + " was destroyed");

        std::vector<Move> moves;
        Log::log(std::string("Alive ships: ") + std::to_string(map.ship_map.at(this->id).size()));

        /** TO TEST ROUTER, UNCOMMENT THIS AND COMMENT ALL CODE BELOW **

        std::vector<const Ship*> swarm;

        for (auto& drone : this-> my_drones) {
            swarm.push_back(&map.get_ship(id, drone.second->ship.entity_id));
        }

        const hlt::Location* target = &map.planets[0].location;
        router->move(&moves, swarm, target);

        */
        for (const Ship& ship : map.ships.at(this->id)) {
            if (ship.docking_status != ShipDockingStatus::Undocked) {
                continue;
            }

            for (const Planet& planet : map.planets) {
                if (planet.owned) {
                    continue;
                }

                if (ship.can_dock(planet)) {
                    moves.push_back(Move::dock(ship.entity_id, planet.entity_id));
                    break;
                }

                const possibly<Move> move =
                        navigation::navigate_ship_to_dock(map, ship, planet, constants::MAX_SPEED * 4 / 5);
                if (move.second) {
                    moves.push_back(move.first);
                }

                break;
            }
        }

        return moves;
    }
};
