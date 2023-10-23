#!/bin/bash

./sim_cache 16 16384 1 0 0 gcc_trace.txt > 1.result.txt
./sim_cache 128 2048 8 0 1 go_trace.txt > 2.result.txt
./sim_cache 32 4096 4 0 1 perl_trace.txt > 3.result.txt
./sim_cache 64 8192 2 1 0 gcc_trace.txt > 4.result.txt
./sim_cache 32 1024 4 1 1 go_trace.txt > 5.result.txt

# func to check diff and print OK or FAIL in color
function check_diff {
    diff -iw $1 $2 > /dev/null
    if [ $? -eq 0 ]; then
        echo -e "[\e[32mOK\e[0m]" "$3"
    else
        echo -e "[\e[31mFAIL\e[0m]" "$3\n"
        diff -iw $1 $2
    fi
}

# check diff
check_diff 1.result.txt ../validation/ValidationRun1.txt "ValidationRun1"
check_diff 2.result.txt ../validation/ValidationRun2.txt "ValidationRun2"
check_diff 3.result.txt ../validation/ValidationRun3.txt "ValidationRun3"
check_diff 4.result.txt ../validation/ValidationRun4.txt "ValidationRun4"
check_diff 5.result.txt ../validation/ValidationRun5.txt "ValidationRun5"

