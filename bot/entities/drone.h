#pragma once

#include "../../hlt/ship.hpp"
#include "tag_list.h"
#include "entity.h"


class Drone : public Entity
{
public:
    hlt::Ship ship;


public:
    explicit Drone (hlt::Ship const & ship_)
            : Entity(ship_)
              , ship(ship_)
    { }

    void update (hlt::Ship const & ship)
    {
        Entity::update(ship);
        this->ship = ship;
    }
};
