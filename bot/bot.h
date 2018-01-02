#pragma once

#include <cstddef>
#include <list>
#include <map>
#include <string>


#include "routing/router.h"
#include "../hlt/hlt.hpp"
#include "../hlt/navigation.hpp"
#include "entities/drone.h"
#include "debug/timer.h"


class ForceMap;

class Bot final
{
public:
    int id;

    hlt::Map const * map;

    std::vector<Entity> entities;
    std::map<int, Drone> drones;

    std::map<int, Drone *> my_drones;

    ForceMap * navmap_force;
    Router router;


public:
    explicit Bot (hlt::Metadata data);

    std::vector<hlt::Move> make_turn (const hlt::Map & map);
};
