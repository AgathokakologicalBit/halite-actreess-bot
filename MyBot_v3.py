import hlt_py as hlt
import logging


BOT_INFO = dict(name='Actreess', version=3)


game = hlt.Game('{name}-{version}'.format(**BOT_INFO))
logging.info("Starting the '{name} v{version}' bot!".format(**BOT_INFO))

ships_targets = {}
while True:
    game_map = game.update_map()

    command_queue = []
    target_planets = []
    for ship in game_map.get_me().all_ships():
        if ship.docking_status != ship.DockingStatus.UNDOCKED:
            continue

        if ship in ships_targets:
            planet = ships_targets[ship.id]
            if planet.is_owned() and planet.owner == game_map.get_me():
                ships_targets.pop(ship.id, None)
                if planet in target_planets: target_planets.remove(planet)
                continue

            if ship.can_dock(planet):
                command_queue.append(ship.dock(planet))
                ships_targets.pop(ship.id, None)
                if planet in target_planets: target_planets.remove(planet)
                continue

            navigate_command = ship.navigate(
                ship.closest_point_to(planet),
                game_map,
                speed=int(hlt.constants.MAX_SPEED),
                ignore_ships=True)

            if navigate_command:
                command_queue.append(navigate_command)
                target_planets.append(planet)
                ships_targets[ship.id] = planet
                continue
            else:
                if planet in target_planets: target_planets.remove(ships_targets[ship.id])
                ships_targets.pop(ship.id, None)

        for planet in game_map.all_planets():
            if planet.is_owned() and planet.owner == game_map.get_me():
                continue

            if ship.can_dock(planet):
                command_queue.append(ship.dock(planet))
                if planet in target_planets: target_planets.remove(planet)
                ships_targets.pop(ship.id, None)
                break

            if planet in target_planets:
                continue

            navigate_command = ship.navigate(
                ship.closest_point_to(planet),
                game_map,
                speed=int(hlt.constants.MAX_SPEED),
                ignore_ships=True)

            if navigate_command:
                command_queue.append(navigate_command)
                target_planets.append(planet)
                ships_targets[ship.id] = planet
            break

    game.send_command_queue(command_queue)
