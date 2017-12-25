#pragma once

#include "../hlt/ship.hpp"

class Drone {
public:
    int id;
    hlt::Ship ship;

    unsigned int life_state;

    explicit Drone(const hlt::Ship& ship_)
        : id(ship_.entity_id)
        , ship(ship_)
        , life_state(2)
    { }

    void update(const hlt::Ship& ship)
    {
        this->life_state = 0;
        this->ship = ship;
    }
};