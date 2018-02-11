#!/usr/bin/env bash
function print_tree
{
    if test "$#" -eq 1; then
        sleep $1
    else
        sleep 0.2
    fi
    tree -s $dir/$client_1 $dir/$client_2 $dir/$server
}
# sprawdzamy czy tree jest zainstalowane

if ! test -x "$(command -v tree)"; then
    echo "Zainstaluj program \"tree\""
    exit 1
fi

clear
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
sleep 0.2
tree -s $dir/$client_1 $dir/$client_2 $dir/$server
read -n1 -r

clear
echo "test 2 usuwanie pliku"
rm $plik
echo "Czy plik usunął się u $client_2?"
sleep 0.2
tree -s $dir/$client_1 $dir/$client_2 $dir/$server
read -n1 -r

clear
echo "test 3 tworzenie folderu"
folder="folder"
mkdir $folder
echo "Czy folder utworzył się u $client_2?"
sleep 0.2
tree -s $dir/$client_1 $dir/$client_2 $dir/$server
read -n1 -r

clear
echo "test 4 tworzenie pliku wewnątrz folderu"
cd $folder
ls -l > "plik1"
ls -l > "plik2"
ls -l > "plik3"
echo "Czy plik1 plik2 plik3 utworzyły się u $client_2/$folder?"
sleep 0.2
tree -s $dir/$client_1 $dir/$client_2 $dir/$server
read -n1 -r

clear
echo "test 5 usuwanie folderu z zawartością"
cd ".."
rm -r $folder
echo "Czy $client_2/$folder został poprawnie usunięty?"
sleep 0.2
tree -s $dir/$client_1 $dir/$client_2 $dir/$server
read -n1 -r

clear
echo "test 6 tworzenie pliku, tworzenie folderu, skopiowanie pliku do folderu, usuwanie folderu z zawartością, usuwanie pliku"
ls -l > "pp"
mkdir "tmp"
cp "./pp" "./tmp/pp"
sleep 0.2
tree -s $dir/$client_1 $dir/$client_2 $dir/$server
read -n1 -r -p "Czy plik pp został skopiowany do  $client_2/tmp?"
rm pp
rm -r tmp

clear
read -n1 -r -p "wyłącz, a następnie uruchom ponownie $client_2"

clear
echo "test7 tworzenie pliku, tworzenie folderu, mv pliku do folderu, usuniecie folderu z zawartoscia"
ls -l > "abc"
mkdir "ff"
mv abc "./ff/"
sleep 0.2
tree -s $dir/$client_1 $dir/$client_2 $dir/$server
read -n1 -r -p "Czy plik abc został przeniesiony do folderu ff?"
rm -r ff

clear
echo "test 8 przenoszenie pliku z zewnątrz do wewnątrz"
cd $dir
dd if=/dev/zero of=pz bs=100 count=1
mv "pz" "$dir/$client_1/pz"
sleep 0.2
tree -s $dir/$client_1 $dir/$client_2 $dir/$server
read -n1 -r -p "Czy plik pz został poprawnie przeniesiony z zewnątrz?"
rm "$dir/$client_1/pz"
clear

clear
echo "test 9 tworzenie pliku, modyfikacja pliku, a nastepnie jego usuniecie"
cd $dir/$client_1
size=10018
dd if=/dev/zero of=big bs=$size count=1
print_tree 0.5
read -n1 -r -p "Czy plik \"big\" o rozmiarze $size został utworzony?"
clear

ls -l >> big
sleep 0.2
tree -s $dir/$client_1 $dir/$client_2 $dir/$server
read -n1 -r -p "Czy plik został zmodyfikowany?"
clear

rm big
sleep 0.2
tree -s $dir/$client_1 $dir/$client_2 $dir/$server
read -n1 -r -p "Czy plik został usuniety?"
clear

