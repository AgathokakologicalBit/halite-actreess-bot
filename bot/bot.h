#pragma once

#include <cstddef>
#include <hlt.hpp>
#include <list>
#include "drone.h"

class Bot final {
public:
    std::size_t id;

    std::list<Drone> drones;

    Bot(hlt::Metadata data)
            : id(data.player_id) {

    }
};