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
    GREEN='\033[0m'    RED='\033[0m'
    NC='\033[0m'
fi

SCORE=0

echo -e "\nStart testing"
remake
echo -e "\nTesting :: Compilation\n"
echo -e "  ${GREEN}Passed${NC}" # if you've made it this far, it's compiling
SCORE=$(($SCORE+10))

remake
echo -e "\nTesting :: Correct Output for Sections 1 and 3\n"
cp ./test-files/count.txt ./test-files/add.txt
echo "./test-files/2000.csv" >> ./test-files/add.txt
RES=$(. ./test-files/add.txt)
# if at least 2 matches (supposedly 2 or 3) to the correct final balance in final output, pass
if [ `./Teller -i ./test-files/2000.csv 2>/dev/null | grep -- "${RES}" | wc -l` -ge 2 ] 
    then
        echo -e "  ${GREEN}Passed${NC}"
        SCORE=$(($SCORE+33))
else
    echo -e "  ${RED}Failed${NC}"
fi

remake
echo -e "\nTesting :: Use of Threads\n"
strace -o out.trace ./Teller -i ./test-files/2000.csv 2>&1 >/dev/null
if cat out.trace |  grep "clone" | grep -qF "CLONE_THREAD"; then
        echo -e "  ${GREEN}Passed${NC}"
        SCORE=$(($SCORE+24))
else
    echo -e "  ${RED}Failed${NC}"
fi


# print score and delete executable
echo -e "\nSCORE: ${SCORE}/67\n"
make -s clean

exit 0