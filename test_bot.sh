#!/usr/bin/env bash

COLOR_RED='\033[1;31m'
COLOR_GREEN='\033[1;32m'
COLOR_END='\033[0m'

PLAYERS_COUNT=2
TEST_COUNT=50

SCORE=0
SCORE_MAX=$(( TEST_COUNT * (TEST_COUNT ** (PLAYERS_COUNT - 1) - 1) ))

rm *.log 2>/dev/null
rm *.hlt 2>/dev/null

for (( i=1; i<=TEST_COUNT; i++ ))
do
    GAME_RES=$(./run_game.sh | tail -n 2 | head -n 1 | awk '{ print $7 }')
    GAME_RES=${GAME_RES#"#"}
    echo ${GAME_RES}th place out of ${PLAYERS_COUNT}
    SCORE=$(( SCORE + TEST_COUNT ** (GAME_RES - 1) - 1 ))
done

ERRORS_COUNT=$( find \d+-*.log 2>/dev/null | wc -l )
SCORE=$(( SCORE - ERRORS_COUNT * TEST_COUNT ** PLAYERS_COUNT ))

echo
if [[ ERRORS_COUNT -gt 0 ]]
then
    printf "${COLOR_RED}${ERRORS_COUNT} error occurred!${COLOR_END}\n"
else
    printf "${COLOR_GREEN}no errors occurred${COLOR_END}\n"
fi

echo scored: $(( SCORE_MAX - SCORE )) out of ${SCORE_MAX}
echo result: $((100 - SCORE * 100 / SCORE_MAX))%

rm *_*.log 2>/dev/null
rm *.hlt 2>/dev/null
