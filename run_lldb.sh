#!/bin/bash
lldb -b -o "run" -o "process interrupt" -o "thread backtrace all" -o "quit" ./build/tests/gj_test_e2e &
PID=$!
sleep 5
kill -INT $PID
wait $PID
