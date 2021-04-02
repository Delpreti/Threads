#! /bin/bash

for choice in {1,2}; do
    for size in {500,1000,2000}; do
        for threads in {1,2,4}; do
            for i in {1..5}; do
                ./out "$size" "$threads" "$choice"
            done
        done
    done
done
