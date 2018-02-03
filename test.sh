#!/usr/bin/env bash

# nazwy poszczególnych folderów

if test "$#" -ne 3; then
    printf "Umieść skrypt w folderze zawierającym katalogi klientów i serwera\nUruchamiaj w następujący sposób:\n./test.sh folder_klienta_1 folder_klienta_2 folder_serwera\n"
            exit
fi
client_1=$1
client_2=$2
server=$3
dir="$(pwd)"

clear
echo "czyścimy foldery"
for f in $client_1 $client_2 $server 
do
    rm -r $dir/$f/* 2> /dev/null
done
read -n1 -r -p "Wciśnij enter by rozpocząć test"

clear
echo "test 1 tworzenie pliku"
plik="plik"
cd $client_1
ls -l > "plik"
echo "Czy plik pojawił się u $client_2?"
read -n1 -r

clear
echo "test 2 usuwanie pliku"
rm $plik
echo "Czy plik usunął się u $client_2?"
read -n1 -r

clear
echo "test 3 tworzenie folderu"
folder="folder"
mkdir $folder
echo "Czy folder utworzył się u $client_2?"
read -n1 -r

clear
echo "test 4 tworzenie pliku wewnątrz folderu"
cd $folder
ls -l > "plik1" 
ls -l > "plik2" 
ls -l > "plik3"
echo "Czy plik1 plik2 plik3 utworzyły się u $client_2/$folder?"
read -n1 -r

clear
echo "test 5 usuwanie folderu z zawartością"
cd ".."
rm -r $folder
echo "Czy $client_2/$folder został poprawnie usunięty?"
read -n1 -r

clear
echo "test 6 tworzenie pliku, tworzenie folderu, kopiowanie pliku do folderu, usuwanie folderu z zawartością, usuwanie pliku"
ls -l > "pp"
mkdir "tmp"
cp "./pp" "./tmp/pp"
read -n1 -r -p "Czy plik pp został skopiowany do  $client_2/tmp?"
rm pp
rm -r tmp

clear
echo "test7 tworzenie pliku, tworzenie folderu, mv pliku do folderu, usuniecie folderu z zawartoscia"
ls -l > "abc"
mkdir "ff"
mv abc "./ff/"
read -n1 -r -p "Czy plik abc został przeniesiony do folderu ff?"
rm -r ff

clear
echo "test 8 przenoszenie pliku z zewnątrz do wewnątrz"
cd $dir
dd if=/dev/zero of=pz bs=100 count=1
mv "pz" "$dir/$client_1/pz"
read -n1 -r -p "Czy plik pz został poprawnie przeniesiony z zewnątrz?"
clear

clear
echo "test 9 tworzenie pliku, modyfikacja pliku, a nastepnie jego usuniecie"
cd $dir/$client_1
dd if=/dev/zero of=big bs=2045 count=1
read -n1 -r -p "Czy plik \"big\" o rozmiarze 2045b został utworzony?"
ls -l >> big 
read -n1 -r -p "Czy plik został zmodyfikowany?"
rm big
read -n1 -r -p "Czy plik został usuniety?"
clear

