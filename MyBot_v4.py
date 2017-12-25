import hlt_py as hlt
import logging


BOT_INFO = dict(name='Actreess', version=4)


game = hlt.Game('{name}-{version}'.format(**BOT_INFO))
logging.info("Starting the '{name} v{version}' bot!".format(**BOT_INFO))

while True:
    game_map = game.update_map()

    command_queue = []
    planet_list = list(p for p in game_map.all_planets() if p.owner is None)
    if not planet_list:
        planet_list = list(p for p in game_map.all_planets() if p.owner is not game_map.get_me())
    if not planet_list:
        planet_list = game_map.all_planets()
    if not planet_list:
        continue
    
    for index, ship in enumerate(game_map.get_me().all_ships()):
        if ship.docking_status != ship.DockingStatus.UNDOCKED:
            continue

        planet = planet_list[index % len(planet_list)]
        if planet.is_owned() and planet.owner == game_map.get_me():
            continue

        if ship.can_dock(planet):
            command_queue.append(ship.dock(planet))
        else:
            navigate_command = ship.navigate(
                ship.closest_point_to(planet),
                game_map,
                speed=int(hlt.constants.MAX_SPEED),
                ignore_ships=True)

            if navigate_command:
                command_queue.append(navigate_command)

    game.send_command_queue(command_queue)
