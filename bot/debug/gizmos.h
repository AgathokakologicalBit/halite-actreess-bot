#pragma once

#define DEBUG_MODE

#include <cstddef>
#include "../../hlt/log.hpp"


static std::uint8_t width = 1;
static std::uint32_t color = 0xFFFFFF;
static std::uint16_t dash;

class Gizmos
{
public:
    template <typename T, typename U>
    static void line (T const & start, U const & end)
    {
#ifdef DEBUG_MODE
        hlt::Log::log(
                "[Line] " +
                std::to_string(::width) + ' ' +
                std::to_string(::color) + ' ' +
                std::to_string(::dash) + ' ' +
                std::to_string(static_cast<std::size_t>(start.x + .5)) + ' ' +
                std::to_string(static_cast<std::size_t>(start.y + .5)) + ' ' +
                std::to_string(static_cast<std::size_t>(end.x + .5)) + ' ' +
                std::to_string(static_cast<std::size_t>(end.y + .5))
        );
#endif
    }

    template <typename T, typename U>
    static void line (T * const start, U * const end)
    {
        Gizmos::line(*start, *end);
    }

    static void set_width (std::uint8_t width)
    { ::width = width; }

    static void set_color (std::uint32_t color)
    { ::color = color; }

    static void set_dash (std::uint8_t fill, std::uint8_t gap)
    { ::dash = (static_cast<std::uint16_t>(fill) << 8) | gap; }

    static std::uint32_t get_color ()
    { return ::color; }

    static std::uint16_t get_dash_fill ()
    { return static_cast<std::uint16_t>(::dash >> 8); }

    static std::uint16_t get_dash_gap ()
    { return static_cast<std::uint16_t>(::dash | ((1 << 8) - 1)); }


    static std::uint32_t color_from_rgb(std::uint8_t r, std::uint8_t g, std::uint8_t b)
    { return (static_cast<std::uint32_t>(r) << 16) | (static_cast<std::uint32_t>(g) << 8) | b; }
};
