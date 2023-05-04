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
echo -e "  ${GREEN}Test One Passed${NC}" # if you've made it this far, it's compiling
SCORE=$(($SCORE+10))

# if outputs differ from expected output files, then incorrect output (2 test files)
remake
echo -e "\nTesting :: Correct Output Pt. 1\n"
cat ./test-files/test_output.txt ./test-files/test_exit.txt > ./test-files/cmd.txt
./data_entry -c "Boring Names 777" -f "./test-files/cmd.csv" < ./test-files/cmd.txt 2>&1 >/dev/null
if diff ./test-files/cmd.csv ./test-files/test_output.csv >/dev/null; then
    echo -e "  ${GREEN}Test Two Passed${NC}"
    SCORE=$(($SCORE+16))
else
    echo -e "  ${RED}Failed${NC}"
fi

remake
echo -e "\nTesting :: Correct Output Pt. 2\n"
cat ./test-files/test_output2.txt ./test-files/test_exit.txt > ./test-files/cmd.txt
./data_entry -c "Name ID Age" -f "./test-files/cmd.csv" < ./test-files/cmd.txt 2>&1 >/dev/null
if diff ./test-files/cmd.csv ./test-files/test_output2.csv >/dev/null; then
    echo -e "  ${GREEN}Test Three Passed${NC}"
    SCORE=$(($SCORE+17))
else
    echo -e "  ${RED}Failed${NC}"
fi

# if strace clones thread at least twice, then passes
remake
echo -e "\nTesting :: Use of Threads\n"
cat ./test-files/test_threads.txt ./test-files/test_exit.txt > ./test-files/cmd.txt
strace >/dev/null -o out.trace ./data_entry -c "Emote Num1 Num2" -f "./test-files/cmd.csv" < ./test-files/cmd.txt 2>&1 >/dev/null
if [ `cat out.trace |  grep "clone" | grep "CLONE_THREAD" | wc -l` -ge 2 ]; then
    echo -e "  ${GREEN}Test Four Passed${NC}"
    SCORE=$(($SCORE+22))
else
    echo -e "  ${RED}Failed${NC}"
fi

# if code has to kill thread itself, then fail
remake
echo -e "\nTesting :: Threads Close Correctly\n"
cat ./test-files/test_threads.txt ./test-files/test_exit.txt > ./test-files/cmd.txt
strace >/dev/null -o out.trace ./data_entry -c "Emote Num1 Num2" -f "./test-files/cmd.csv" < ./test-files/cmd.txt 2>&1 >/dev/null
if cat out.trace | egrep -q "tgkill|tkill"; then
    echo -e "  ${RED}Failed${NC}"
else
    echo -e "  ${GREEN}Test Five Passed${NC}"
    SCORE=$(($SCORE+22))
fi

# if returns a fail then memory leak
remake
echo -e "\nTesting :: Memory Leakage\n"
cat ./test-files/test_output.txt ./test-files/test_exit.txt > ./test-files/cmd.txt
if ./data_entry -c "Emote Num1 Num2" -f "./test-files/cmd.csv" < ./test-files/cmd.txt 2>&1 >/dev/null; then
    echo -e "  ${GREEN}Test Six Passed${NC}"
    SCORE=$(($SCORE+13))
else
    echo -e "  ${RED}Failed${NC}"
fi

# print score and delete executable
echo -e "\nSCORE: ${SCORE}/100\n"
make -s clean

exit 0
