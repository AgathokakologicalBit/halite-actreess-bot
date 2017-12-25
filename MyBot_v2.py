import hlt_py as hlt
import logging


BOT_INFO = dict(name='Actreess')


game = hlt.Game(BOT_INFO['name'])
logging.info("Starting the '{name}' bot!".format(name=BOT_INFO['name']))

while True:
    game_map = game.update_map()

    command_queue = []
    target_planets = []
    for ship in game_map.get_me().all_ships():
        # If the ship is docked
        if ship.docking_status != ship.DockingStatus.UNDOCKED:
            # Skip this ship
            continue

        # For each planet in the game (only non-destroyed planets are included)
        for planet in game_map.all_planets():
            # If the planet is owned
            if planet.is_owned():
                # Skip this planet
                continue

            # If we can dock, let's (try to) dock. If two ships try to dock at once, neither will be able to.
            if ship.can_dock(planet):
                command_queue.append(ship.dock(planet))
            else:
                if planet in target_planets:
                    continue

                target_planets.append(planet)
                navigate_command = ship.navigate(
                    ship.closest_point_to(planet),
                    game_map,
                    speed=int(hlt.constants.MAX_SPEED),
                    ignore_ships=True)

                # If the move is possible, add it to the command_queue (if there are too many obstacles on the way
                # or we are trapped (or we reached our destination!), navigate_command will return null;
                # don't fret though, we can run the command again the next turn)
                if navigate_command:
                    command_queue.append(navigate_command)
            break

    game.send_command_queue(command_queue)
