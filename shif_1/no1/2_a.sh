#!/bin/bash

# Mendapatkan jam sekarang
jam=$(date +%H)

# Membuat folder kumpulan
mkdir "kumpulan$jam"

# Mendownload gambar
for ((i=1; i<=$jam; i++))
do
    wget -O "kumpulan$jam/perjalanan_$i" "https://upload.wikimedia.org/wikipedia/commons/f/f1/Indonesia_declaration_of_independence_17_August_1945.jpg"
done

# Membuat cron job untuk menjalankan script setiap 10 jam
crontab -l > mycron
echo "0 */10 * * * /bin/bash /home/2_a.sh" >> mycron

# Menjalankan cron job
crontab mycron
