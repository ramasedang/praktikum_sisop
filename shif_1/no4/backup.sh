#!/bin/bash
#This script is used to backup files with the format hour:minute date:month:year

#Define the date and time variables
date=$(date +"%H:%M%d:%m:%Y")

#Define the cipher
cipher=$(date +"%H")

#Backup the syslog file
cp /var/log/syslog /var/log/syslog$date

#Encrypt the backup
for i in $(cat /var/log/syslog$date); do
    #Loop through the cipher
    for (( j=1; j<=$cipher; j++ )); do
        #Substitute the characters
        i=$(echo "$i" | tr "abcdefghijklmnopqrstuvwxyz" "bcdefghijklmnopqrstuvwxyza")
    done
    #Write the encrypted file
    echo "$i" >> /var/log/syslog$date.enc
done

#Schedule the script to run every two hours
# crontab -e
# 0 /2 * * /path/to/backup_syslog.sh