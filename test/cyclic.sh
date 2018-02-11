#!/usr/bin/env bash

if test "$#" -ne 1; then
    echo "podaj folder klienta 1"
    exit
fi
cd $1
mkdir folder
cd folder
pwd
files=("plik1.txt" "plik2.txt" "plik3.txt")
while true
do
    mkdir f
    cd f
    for file in ${files[*]}
    do
        ls -al > $file
    done
    sleep 3
    for file in ${files[*]}
    do
        rm $file
    done
    sleep 3
    cd ..
    rm f
done
