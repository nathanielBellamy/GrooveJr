#!/bin/bash
./build/tests/gj_test_e2e &
PID=$!
sleep 5
kill -QUIT $PID
wait $PID
