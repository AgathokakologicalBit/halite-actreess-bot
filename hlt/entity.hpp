#pragma once

#include "location.hpp"
#include "types.hpp"

namespace hlt
{
    struct Entity
    {
        EntityId entity_id;
        PlayerId owner_id;
        Location location;

        int health;
        double radius;

        Entity() = default;
        Entity (Location location, double radius)
                : entity_id(0)
                  , owner_id(0)
                  , location(location)
                  , health(0)
                  , radius(radius)
        { }

        bool is_alive () const
        {
            return health > 0;
        }
    };
}
