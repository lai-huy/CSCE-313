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

SCORE=0

echo -e "\nStart testing"
remake
echo -e "\nTesting :: Compilation\n"
echo -e "  ${GREEN}Test One Passed${NC}" # if you've made it this far, it's compiling
SCORE=$(($SCORE+10))

remake
echo -e "\nTesting :: Memory Management\n"
if ./shell < ./test-files/test_exit.txt 2>/dev/null; then
    echo -e "  ${GREEN}Test Two Passed${NC}"
    SCORE=$(($SCORE+10))
else
    echo -e "  ${RED}Failed${NC}"
fi

remake
echo -e "\nTesting :: Correct Output For Multiple Lines\n"
cat ./test-files/test_multiple_lines.txt ./test-files/test_exit.txt > ./test-files/cmd.txt
RES=$(. ./test-files/test_multiple_lines.txt)
if ./shell < ./test-files/cmd.txt 2>/dev/null | grep -qF -- "${RES}"; then
    echo -e "  ${GREEN}Test Three Passed${NC}"
    SCORE=$(($SCORE+25))
else
    echo -e "  ${RED}Failed${NC}"
fi

remake
echo -e "\nTesting :: Correct Output For 2 Commands\n"
cat ./test-files/test_two_commands.txt ./test-files/test_exit.txt > ./test-files/cmd.txt
RES=$(. ./test-files/test_two_commands.txt)
if ./shell < ./test-files/cmd.txt 2>/dev/null | grep -qF -- "${RES}"; then
    echo -e "  ${GREEN}Test Four Passed${NC}"
    SCORE=$(($SCORE+25))
else
    echo -e "  ${RED}Failed${NC}"
fi

remake
echo -e "\nTesting :: Correct Output For 3+ Commands\n"
cat ./test-files/test_four_commands.txt ./test-files/test_exit.txt > ./test-files/cmd.txt
RES=$(. ./test-files/test_four_commands.txt)
if ./shell < ./test-files/cmd.txt 2>/dev/null | grep -qF -- "${RES}"; then
    echo -e "  ${GREEN}Test Five Passed${NC}"
    SCORE=$(($SCORE+15))
else
    echo -e "  ${RED}Failed${NC}"
fi


# print score and delete executable
echo -e "\nSCORE: ${SCORE}/85\n"
make -s clean

exit 0
