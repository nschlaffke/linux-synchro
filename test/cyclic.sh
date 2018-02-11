#!/usr/bin/env bash
if test "$#" -ne 1; then
    echo "podaj folder klienta 1"
    exit
fi
cd $1
pwd
files=("plik1.txt" "plik2.txt" "plik3.txt")
while true
do
    for file in ${files[*]}
    do
        ls -al > $file
    done
    sleep 1
    for file in ${files[*]}
    do

        rm $file
    done
    sleep 1
done
