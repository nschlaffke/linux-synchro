#!/usr/bin/env bash

if test "$#" -ne 1; then
    echo "podaj folder klienta 1"
    exit
fi

files=("plik1.txt", "plik2.txt", "plik3.txt")
while true
do
    for file in ${array[*]}
    do
        ls -al >> file 
    done
    for file in ${array[*]}
    do
        rm file
    done
done
