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

function make_file
{
    if test "$#" -ne 2; then
        clear
        printf "make_file nazwa_pliku rozmiar_w_bajtach"
        exit
    fi
    dd if=/dev/zero of=$1 bs=$2 count=1 2>/dev/null
}

# sprawdzamy czy tree jest zainstalowane

if ! test -x "$(command -v tree)"; then
    echo "Zainstaluj program \"tree\""
    exit 1
fi

# sprawdzamy czy podana została poprawna liczba argumentów
clear
if test "$#" -ne 3; then
    printf "Umieść skrypt w folderze zawierającym katalogi klientów i serwera\n
            Uruchamiaj w następujący sposób:\n
            ./test.sh folder_klienta_1 folder_klienta_2 folder_serwera\n"
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

echo "tworzymy pliki w folderach..."
make_file $dir/$client_1/p2 22
make_file $dir/$client_1/p3 3
mkdir $dir/$client_1/fff
make_file $dir/$client_1/fff/bbb 99
mkdir $dir/$client_1/oko
make_file $dir/$server/p4 4
sleep 1

mkdir $dir/$server/fff
make_file $dir/$server/fff/sss 77 
make_file $dir/$client_2/p1 1
make_file $dir/$client_2/p4 44
sleep 1

make_file $dir/$server/p3 33
make_file $dir/$server/p5 55
mkdir $dir/$client_2/oko
make_file $dir/$client_2/oko/pp 88
make_file $dir/$client_1/p1 11

clear
echo "Uruchom serwer i klientów"
read -n1 -r
sleep 1
clear
echo "Rozmiary plików powinny być dwucyfrowe"
print_tree 1
read -n1 -r
clear

echo "Usuwanie foldera fff u $client_1"
rm -r $dir/$client_1/fff
print_tree 1
echo "Czy folder fff został poprawnie usunięty?"
read -n1 -r

echo "Usuwanie foldera oko u $client_2"
rm -r $dir/$client_2/oko
print_tree 1
echo "Czy folder fff został poprawnie usunięty?"
read -n1 -r
