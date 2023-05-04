#!/bin/bash 
./p1 -n $1 2>/dev/null > ./test-files/tmp.txt

pids=$(head -1 ./test-files/tmp.txt)

OIFS=$IFS
IFS=$' '           ## only word-split on '\n'
pids_sorted=( $(printf "%s\n" ${pids[@]} | sort -r ) )  ## reverse sort

echo -e $pids > ./test-files/soln.txt

IFS=$'\n'
for s in $pids_sorted; do 
    echo -e "Process $s: hello" >> ./test-files/soln.txt
    echo -e "Process $s: exiting" >> ./test-files/soln.txt
done 
IFS=$OIFS

echo -e "Parent: exiting" >> ./test-files/soln.txt

if diff ./test-files/tmp.txt ./test-files/soln.txt; then 
    echo 1
else 
    echo 0
fi 