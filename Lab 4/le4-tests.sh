#!/usr/bin/env bash

# function to clean up files and make executables
remake () {
    # echo -e "\ old files and making executables"
    make -s clean
    make -s >/dev/null 2>&1
}


echo -e "To remove colour from tests, set COLOUR to 1 in sh file\n"
COLOUR=0
if [[ COLOUR -eq 0 ]]; then
    ORANGE='\033[0;33m'
    GREEN='\033[0;32m'
    RED='\033[0;31m'
    NC='\033[0m'
else
    ORANGE='\033[0m'
    GREEN='\033[0m'
    RED='\033[0m'
    NC='\033[0m'
fi

SCORE=10

# if you made it here then it compiled lol
echo -e "\nStart testing"
echo -e "\nTesting :: Compilation\n"
echo -e "  ${GREEN}Test One Passed${NC}"

remake
echo -e "\nTesting :: Memory Leakage\n"
if ./schedule -i test-files/test_short.csv -q 3 2>/dev/null 1>/dev/null; then
    echo -e "  ${GREEN}Test Two Passed${NC}"
    SCORE=$(($SCORE+10))
else
    echo -e "  ${RED}Failed${NC}"
fi

remake
echo -e "\nTesting :: Scheduling with Jobs Shorter Than Quantums pt 1\n"
./schedule -i test-files/test_short.csv -q 100 > test-files/cmd.txt 2>/dev/null
if diff -q test-files/cmd.txt test-files/output_short.txt 2>/dev/null; then
    echo -e "  ${GREEN}Test Three Passed${NC}"
    SCORE=$(($SCORE+15))
else
    echo -e "  ${RED}Failed${NC}"
fi

# this includes a 0 length test case
remake
echo -e "\nTesting :: Scheduling with Jobs Shorter Than Quantums pt 2\n"
./schedule -i test-files/test_v_short.csv -q 2 > test-files/cmd.txt 2>/dev/null
if diff -q test-files/cmd.txt test-files/output_v_short.txt 2>/dev/null; then
    echo -e "  ${GREEN}Test Four Passed${NC}"
    SCORE=$(($SCORE+15))
else
    echo -e "  ${RED}Failed${NC}"
fi

remake
echo -e "\nTesting :: Scheduling with Jobs Longer Than Quantums pt 1\n"
./schedule -i test-files/test_long.csv -q 3 > test-files/cmd.txt 2>/dev/null
if diff -q test-files/cmd.txt test-files/output_long.txt 2>/dev/null; then
    echo -e "  ${GREEN}Test Five Passed${NC}"
    SCORE=$(($SCORE+15))
else
    echo -e "  ${RED}Failed${NC}"
fi

# will test for lots of processes, as well as processes that
# get multiple turns in a row
remake
echo -e "\nTesting :: Scheduling with Jobs Longer Than Quantums pt 2\n"
./schedule -i test-files/test_v_long.csv -q 2 > test-files/cmd.txt 2>/dev/null
if diff -q test-files/cmd.txt test-files/output_v_long.txt 2>/dev/null; then
    echo -e "  ${GREEN}Test Six Passed${NC}"
    SCORE=$(($SCORE+15))
else
    echo -e "  ${RED}Failed${NC}"
fi


remake
echo -e "\nTesting :: Scheduling with NOP\n"
./schedule -i Tasks/Task3.csv -q 3 > test-files/cmd.txt 2>/dev/null
if diff -q test-files/cmd.txt test-files/output_nop.txt 2>/dev/null; then
    echo -e "  ${GREEN}Test Seven Passed${NC}"
    SCORE=$(($SCORE+20))
else
    echo -e "  ${RED}Failed${NC}"
fi

# print score and delete executable
echo -e "\nSCORE: ${SCORE}/100\n"
make -s clean

exit 0
