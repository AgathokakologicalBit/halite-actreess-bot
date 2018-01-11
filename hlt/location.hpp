#pragma once

#include <ostream>

#include "constants.hpp"
#include "util.hpp"

namespace hlt
{
    struct Location
    {
        double x, y;


        Location () = default;

        Location (double x, double y)
                : x(x)
                  , y(y)
        { }


        double get_distance_to (const Location & target) const
        {
            const double dx = x - target.x;
            const double dy = y - target.y;
            return std::sqrt(dx * dx + dy * dy);
        }

        int orient_towards_in_deg (const Location & target) const
        {
            return util::angle_rad_to_deg_clipped(orient_towards_in_rad(target));
        }

        double orient_towards_in_rad (const Location & target) const
        {
            const double dx = target.x - x;
            const double dy = target.y - y;

            return std::atan2(dy, dx) + 2 * M_PI;
        }

        Location get_closest_point (const Location & target, const double target_radius) const
        {
            const double radius = target_radius + constants::MIN_DISTANCE_FOR_CLOSEST_POINT;
            const double angle_rad = target.orient_towards_in_rad(*this);

            const double x = target.x + radius * std::cos(angle_rad);
            const double y = target.y + radius * std::sin(angle_rad);

            return Location(x, y);
        }

        friend std::ostream & operator << (std::ostream & out, const Location & location);
    };

    static bool operator == (const Location & l1, const Location & l2)
    {
        return l1.x == l2.x && l1.y == l2.y;
    }
}