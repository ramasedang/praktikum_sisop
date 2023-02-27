#!/bin/bash

# Mendefinisikan path file log system yang akan di-backup dan enkripsi
LOG_FILE_PATH="/var/log/syslog"
ENCRYPTED_LOG_FILE_PATH="encrypted.log"

# Mendapatkan waktu saat ini
CURRENT_TIME=$(date +"%H:%M %d:%m:%Y")

# Mendapatkan jam dan menit dari waktu saat ini
CURRENT_HOUR=$(date +"%H")
CURRENT_MINUTE=$(date +"%M")

# Membuat string untuk digunakan sebagai key dalam enkripsi
KEY=$(printf '%s' "$CURRENT_HOUR$CURRENT_MINUTE")

# Membaca isi file log system
LOG_FILE_CONTENT=$(cat "$LOG_FILE_PATH")

# Melakukan enkripsi pada isi file log system menggunakan sistem cipher
ENCRYPTED_LOG_FILE_CONTENT=$(echo "$LOG_FILE_CONTENT" | tr 'a-zA-Z' "$(printf %${KEY}s | tr ' ' '.')a-zA-Z" | tr 'a-zA-Z' 'n-za-mN-ZA-M')

# Menulis isi file log system yang telah dienkripsi ke file baru
echo "$ENCRYPTED_LOG_FILE_CONTENT" > "$ENCRYPTED_LOG_FILE_PATH"

# Mencetak pesan bahwa backup dan enkripsi berhasil dilakukan
echo "Backup dan enkripsi file log system berhasil dilakukan pada $CURRENT_TIME"