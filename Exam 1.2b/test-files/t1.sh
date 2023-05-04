#!/bin/bash 
strace -e trace=write -f -o trace.tst ./p2 "$1" 2>/dev/null > ./test-files/tmp.txt

h1=$(head -1 ./test-files/tmp.txt)
h2=$(tail -1 ./test-files/tmp.txt)

if [ $(wc -l < ./test-files/tmp.txt) -eq 2 ]; then 
    # check if the hashes match // TODO: compute hash again here and match
    if [ $h1 = $h2 ]; then 
        # check if all prints coming from the parent
        parent_id=$(grep "write.*$1" trace.tst | awk '{print $1}')
        for pid in $(grep "write(1" trace.tst | awk '{print $1}'); do 
            if [ $parent_id != $pid ]; then 
                echo 0
                exit 
            fi 
        done 
        echo 1
        exit 
    fi 
fi 
echo 0
