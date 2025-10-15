#!/bin/bash
# Proceed with tests
echo starting tests
#regular testers
./clientEx "-out (foo, bar)"
./clientEx "-rd (foo, ?)"
./clientEx "-in (foo, ?)"

#concurrency testers
./clientEx "-rd (?, ?)" &
./clientEx "-out (test, succeeded)"

./clientEx "-in (?, ?, ?)" &
./clientEx "-out (this, also, works)"

./clientEx "-kill"