#!/bin/sh

set -e

cmake .
make MyBot

./halite -d "240 160" "./MyBot" "python3 MyBot_v1.py"