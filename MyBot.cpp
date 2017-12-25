#include <bot/bot.h>

int main() {
    using namespace hlt;

    Bot bot(initialize("Actreess-8"));
    while (true) {
        const Map map = in::get_map();
        auto moves = bot.make_turn(map);
        if (!out::send_moves(moves)) {
            Log::log("send_moves failed; exiting");
            break;
        }
    }
}
