#!/bin/bash

# Mendapatkan jam sekarang
jam=$(date +%H)

# Membuat folder kumpulan
mkdir "kumpulan_$jam"

# Mendownload gambar
for ((i=0; i<=$jam; i++))
do
    if [ $i == 0 ]
    then
        wget -O "kumpulan_$jam/perjalanan_$i" "https://upload.wikimedia.org/wikipedia/commons/f/f1/Indonesia_declaration_of_independence_17_August_1945.jpg"
        break
    fi
    if [ $i == 1 ]
    then
        wget -O "kumpulan_$jam/perjalanan_$i" "https://upload.wikimedia.org/wikipedia/commons/f/f1/Indonesia_declaration_of_independence_17_August_1945.jpg"get -O "kumpulan_$jam/pdkt_kusuma_$i" https://loremflickr.com/320/240/bunny
        break
    else
        wget -O "kumpulan_$jam/perjalanan_$i" "https://upload.wikimedia.org/wikipedia/commons/f/f1/Indonesia_declaration_of_independence_17_August_1945.jpg"
    fi
done

