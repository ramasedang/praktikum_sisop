#!/bin/bash

echo -e "\nTampilkan 5 Universitas dengan ranking tertinggi di Jepang: \n"

grep "Japan" list_univ.csv | sort -n -t , -k 1 | head -n 5 | awk -F "," '{print $1","$2","$3}'

echo -e "\n(fsr score) yang paling rendah diantara 5 Universitas dari hasil filter poin a. : \n"

grep -e "Japan" list_univ.csv | sort  -n -t , -k 1 | head -n 5 | sort -t , -k 9 | awk -F "," '{print $2","$9}'

echo -e "\ncari 10 Universitas di Jepang dengan Employment Outcome Rank(ger rank) paling tinggi. : \n"

grep "Japan" list_univ.csv | sort -n -t , -k 20 | head -n 10 | awk -F "," '{print $2","$20}'

echo -e "\nBantu bocchi mencari universitas tersebut dengan kata kunci keren. : \n"

grep -i "keren" list_univ.csv | awk -F "," '{print $1","$2","$3}'