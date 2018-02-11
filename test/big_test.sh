#!/usr/bin/env bash
 if test "$#" -ne 4; then
     printf "Umieść skrypt w folderze zawierającym katalogi klientów i serwera\nUruchamiaj w         następujący sposób:\n./test.sh folder_klienta_1 folder_klienta_2 folder_serwera\n"
             exit
 fi
 client_1=$1
 client_2=$2
 server=$3
 size=$4
 dir="$(pwd)"

 clear
 echo "test 9 tworzenie pliku, modyfikacja pliku, a nastepnie jego usuniecie"
 cd $dir/$client_1
 dd if=/dev/zero of=big bs=$size count=1
 read -n1 -r -p "Czy plik \"big\" o rozmiarze 2045b został utworzony?"
 ls -l >> big 
 read -n1 -r -p "Czy plik został zmodyfikowany?"
 rm big 
 read -n1 -r -p "Czy plik został usuniety?"
 clear
 

