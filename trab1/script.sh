#! /bin/bash

for difficulty in {1..7}; do
    for threads in {1,2,4,8}; do
        for i in {1..10}; do
            ./threadmine "$difficulty" "$threads";
            sleep 1;
        done
    done
done
