#!/usr/bin/env bash

PLAYERS_COUNT=2
TEST_COUNT=50

SCORE=0
SCORE_MAX=$(( TEST_COUNT * (TEST_COUNT ** (PLAYERS_COUNT - 1) - 1) ))

for (( i=1; i<=$TEST_COUNT; i++ ))
do
    GAME_RES=$(./run_game.sh | tail -n 2 | head -n 1 | awk '{ print $7 }')
    GAME_RES=${GAME_RES#"#"}
    echo ${GAME_RES}th place out of ${PLAYERS_COUNT}
    SCORE=$(($SCORE + TEST_COUNT ** (GAME_RES - 1) - 1))
done

echo scored: $(( SCORE_MAX - SCORE )) out of ${SCORE_MAX}
echo result: $((100 - SCORE * 100 / SCORE_MAX))%
