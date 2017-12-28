#!/bin/sh

set -e

cmake .
make MyBot

./halite -d "240 160" "./MyBot" "./MyBot"
