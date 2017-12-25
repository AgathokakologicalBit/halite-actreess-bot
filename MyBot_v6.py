import hlt
import logging
from collections import namedtuple, OrderedDict


""" VERSIONS INFO
    ===---   v1 "BASE"   ---===
      Initial version taken from examples


    ===---   v2 "SPEED"   ---===
      Increased speed factor by 2
      Added target planets list to prevent
        multiple drones to fly on 1 planet


    ===---   v3 "MEMORY"   ---===
      Ships are now storing memory about
        their targets, so they don't lose
        them when others dies or
        new ones born


    ===---   v4 "SEQUENCES"   ---===
      Each drone is now moving towards
        planet with his index
        wrapped by planets count


    ===---   v5 "PRIORITY"   ---===
      Now planets are sorted
        by their distance from the corner
      Ships are now using their ids
        instead of indexes, so they don't
        lose their target after some
        of them die or new ones
        are created


    ===---   v6 "ROLES"   ---===
      Added version logging
      Now there are different
        kinds of drones with
        different goals

        - Siege/Defender drones
          Are conquering planets
          and defend them from
          opponent's ships
        - Assault drones
          Attacks enemy's ships
          to clear the way, so
          siege drones can
          capture the planet
      Added swarm
        Manages drones population
        Chooses right types
          to spawn on field

"""


Point = namedtuple('Point', ('x', 'y'))


class Drone:
    type = '?'

    def __init__(self, ship):
        self.ship = ship
        self.id = ship.id
        self.position = Point(0, 0)
        pass

    def turn(self, swarm): raise NotImplementedError

    def update_info(self, ship):
        self.ship = ship
        self.position = Point(ship.x, ship.y)
        pass


class SiegeDrone(Drone):
    type = 'siege'

    def __init__(self, ship):
        Drone.__init__(self, ship)

        self.target = None
        pass

    def turn(self, swarm):
        if self.ship.docking_status != hlt.entity.Ship.DockingStatus.UNDOCKED:
            return None

        if self.target is not None:
            planets = swarm.map.all_planets()
            pids = [p.id for p in planets]
            if self.target.id in pids:
                self.target = planets[pids.index(self.target.id)]
                if self.target.owner is not None:
                    self.target = None
            else:
                logging.warning('  Target planet #{} was lost...'.format(self.target.id))
                self.target = None

        if self.target is None:
            planets = sorted(swarm.map.all_planets(), key=self.ship.calculate_distance_between)
            nm_planets = [t for t in planets if t.owner is None]

            logging.info(
                '    Found {} / {} possible target planets'.
                    format(len(nm_planets), len(planets)))

            self.target = nm_planets[0] if nm_planets else None
            if self.target is not None:
                logging.info('    Target planet chosen: {}'.format(self.target.id))

        if self.target is None: return None

        if self.ship.can_dock(self.target):
            return dict(type='dock', target=self.target)

        return dict(type='move', target=self.target)

    def update_info(self, ship):
        super(SiegeDrone, self).update_info(ship)
        pass


class AssaultDrone(Drone):
    type = 'assault'

    def __init__(self, ship):
        Drone.__init__(self, ship)

        self.target = None
        pass

    def turn(self, swarm):
        if self.target is not None:
            enemies = []
            for p in swarm.map.all_players():
                if p is not swarm.my_player:
                    enemies.extend(p.all_ships())

            pids = [p.id for p in enemies]
            if self.target.id in pids:
                self.target = enemies[pids.index(self.target.id)]
            else:
                logging.warning('  Target enemy #{} was lost...'.format(self.target.id))
                self.target = None

        if self.target is None:
            enemies = []
            for p in swarm.map.all_players():
                if p is not swarm.my_player:
                    enemies.extend(p.all_ships())

            enemies = sorted(enemies, key=self.ship.calculate_distance_between)

            logging.info(
                '    Found {} possible target enemies'.
                    format(len(enemies)))

            self.target = enemies[0] if enemies else None
            if self.target is not None:
                logging.info('    Target enemy chosen: {}'.format(self.target.id))

        if self.target is None: return None
        return dict(type='move', target=self.target)

    def update_info(self, ship):
        super(AssaultDrone, self).update_info(ship)
        pass


class Swarm:
    def __init__(self):
        self.drones = []
        self.roles = ((SiegeDrone, 5, 32), (AssaultDrone, 4, 0))

        self.map = None
        self.my_player = None
        pass

    def update(self, gmap):
        self.map = gmap
        self.my_player = gmap.get_me()

        alive_ships = self.my_player.all_ships()
        alive_ships_ids = list(d.id for d in alive_ships)

        for drone in self.drones:
            if drone.id in alive_ships_ids:
                logging.info('  Drone #{} - ALIVE'.format(drone.id))
                drone.update_info(alive_ships[alive_ships_ids.index(drone.id)])
            else:
                logging.info('  Drone #{} - DEAD'.format(drone.id))
                self.drones.remove(drone)

        registered_ids = [d.id for d in self.drones]
        for index, drone_id in enumerate(alive_ships_ids):
            if drone_id not in registered_ids:
                self.register_drone(alive_ships[index])

    def register_drone(self, drone):
        logging.info('  Registering drone #{}'.format(drone.id))
        counts = [(r[0], r[1], r[2], len([d for d in self.drones if isinstance(d, r[0])])) for r in self.roles]
        counts = [c for c in counts if c[3] < c[2] or c[2] == 0]
        logging.info('    Suitable classes({}): [{}]'.format(
            len(counts),
            ', '.join(['"{} - {{c: {}({}/{}), max: {}}}"'.format(
                c[0].type, c[3] // c[1], c[3] % c[1], c[1], c[2] if c[2] else 'oo') for c in counts])))

        choice = min(counts, key=lambda c: c[3] // c[1])[0]
        logging.info('    Chosen type: {}'.format(choice.type))

        bot = choice(drone)
        bot.update_info(drone)
        self.drones.append(bot)
        pass


def play_game(game, bot_info):
    logging.info('Starting the \'{name} v{version}\' bot!'.format(**bot_info))

    swarm = Swarm()
    while True:
        game_map = game.update_map()

        swarm.update(game_map)
        command_queue = []

        for drone in swarm.drones:
            logging.info('  Drone {}({}) is moving'.format(drone.type, drone.id))
            turn = drone.turn(swarm)
            logging.info('    Turn result: {}'.format(turn))

            if turn is None: continue

            if turn['type'] == 'dock':
                command_queue.append(drone.ship.dock(turn['target']))
            elif turn['type'] == 'move':
                navigate_command = drone.ship.navigate(
                    drone.ship.closest_point_to(turn['target']),
                    game_map,
                    speed=int(hlt.constants.MAX_SPEED),
                    ignore_ships=False)

                if navigate_command:
                    command_queue.append(navigate_command)

        game.send_command_queue(command_queue)


BOT_INFO = dict(name='Actreess', version=6)
play_game(hlt.Game('{name}-{version}'.format(**BOT_INFO)), BOT_INFO)
