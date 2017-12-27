#pragma once

#include <vector>
#include <string>
#include <map>

#include "bot/drone.h"
#include "hlt.hpp"

class Router
{
public:
    void move (std::vector<hlt::Move> & moves, std::vector<hlt::Ship const *> & ships, hlt::Location const target)
    {
        using namespace hlt;

        Location center = get_fleet_center(ships);
        auto const centerShip = get_closest_ship(center, ships);
        int const angle = centerShip->location.orient_towards_in_deg(target);

        for (auto ship : ships)
        {
            moves.push_back(Move::thrust(ship->entity_id, hlt::constants::MAX_SPEED, angle));
        }
    }

    /**
     * This algorithm finds the location closest to all the others (not the true center)
     * It takes the average x and y of every ship
     * @param ships      vector containing the id's of the ships to pick a center from
     * @return           location of the middle of the fleet
     */
    static hlt::Location get_fleet_center (std::vector<hlt::Ship const *> & ships)
    {
        hlt::Location center;

        for (auto & ship : ships)
        {
            center.pos_x += ship->location.pos_x;
            center.pos_y += ship->location.pos_y;
        }

        center.pos_x /= ships.size();
        center.pos_y /= ships.size();

        return center;
    }

    /**
     * This algorithm finds the ship closest to a location
     * @param location  The location match to a ship
     * @param ships     A list of ships closest to
     * @return          Ship object closest to point
    */
    static const hlt::Ship * get_closest_ship (hlt::Location location, std::vector<hlt::Ship const *> & ships)
    {
        const hlt::Ship * closest = ships[0];
        double best_distance = closest->location.get_distance_to(location);

        for (auto ship : ships)
            if (ship->location.get_distance_to(location) < best_distance)
                closest = ship;

        return closest;
    }
};
