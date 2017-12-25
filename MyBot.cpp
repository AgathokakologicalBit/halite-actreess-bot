#include <bot/bot.h>
#include <hlt.hpp>
#include "hlt/navigation.hpp"

int main() {
    using namespace hlt;

    const auto metadata = initialize("Actreess");
    const PlayerId player_id = metadata.player_id;

    const Map& initial_map = metadata.initial_map;

    Bot bot(metadata);

    std::vector<Move> moves;
    while (true) {
        moves.clear();
        const Map map = in::get_map();

        for (const Ship& ship : map.ships.at(player_id)) {
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

        if (!out::send_moves(moves)) {
            Log::log("send_moves failed; exiting");
            break;
        }
    }
}
