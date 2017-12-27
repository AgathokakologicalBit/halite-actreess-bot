#include <cmath>

#include "location.hpp"

namespace hlt {
    std::ostream& operator<<(std::ostream& out, const Location& location) {
        out << '(' << location.x << ", " << location.y << ')';
        return out;
    }
}
