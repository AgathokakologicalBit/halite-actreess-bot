#pragma once

#include <cstddef>
#include <list>

#include "drone.h"
#include <hlt.hpp>
#include <navigation.hpp>

#include <string>

class Bot final {
public:
    int id;

    std::vector<Drone*> drones;
    std::vector<Drone*> my_drones;

    explicit Bot(hlt::Metadata data)
            : id(data.player_id)
    {
        drones.reserve(600);
        my_drones.reserve(300);

        for (const auto& p : data.initial_map.ships) {
            for (const auto &s : p.second) {
                Drone *d = new Drone(s);

                this->drones.push_back(d);
                if (this->id == p.first)
                    this->my_drones.push_back(d);
            }
        }
    }

    std::vector<hlt::Move> make_turn(const hlt::Map& map) {
        using namespace hlt;

        std::vector<Move> moves;
        Log::log((std::string("Alive ships: ") + std::to_string(map.ship_map.at(this->id).size())).c_str());
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
                        navigation::navigate_ship_to_dock(map, ship, planet, constants::MAX_SPEED / 2);
                if (move.second) {
                    moves.push_back(move.first);
                }

                break;
            }
        }

        return moves;
    }
};