#!/bin/bash

# Mendapatkan waktu saat ini
waktu=$(date +"%Y-%m-%d %H:%M:%S")

# Mencetak waktu ke layar
echo "Waktu saat ini: $waktu"

# Menyimpan waktu ke dalam file log
echo "$waktu" >> /home/ssudo/Matkul/praktikum_sisop/shif_2/no4/log.txt
