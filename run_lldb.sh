#!/bin/bash
lldb -b -o "run" -o "process interrupt" -o "thread backtrace all" -o "quit" ./build/tests/gj_test_e2e > lldb_output.txt &
PID=$!
sleep 5
kill -INT $PID
wait $PID
cat lldb_output.txt
