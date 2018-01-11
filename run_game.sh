#!/bin/sh

set -e

cmake .
make MyBot

./halite -d "240 160" -s 2678305263 "./MyBot" "./MyBot"
