#!/bin/bash
jam=$(date +'%H')
jam=${jam#0}
syslog=$(cat /var/log/syslog)

encrypted=""
echo ${#syslog}
for ((i=0; i<${#syslog}; i++)); do
    char="${syslog:$i:1}"
    if [[ "$char" =~ [A-Za-z] ]]; then
        if [[ "$char" =~ [A-Z] ]]; then
            charcode=$(( ( $(printf '%d' "'$char") - 65 + $jam) % 26 + 65 ))
            char=$(printf \\$(printf '%03o' "$charcode"))
        else
            charcode=$(( ( $(printf '%d' "'$char") - 97 + $jam) % 26 + 97 ))
            char=$(printf \\$(printf '%03o' "$charcode"))
        fi
    fi
    encrypted="${encrypted}${char}"
done

filename=$(date +'%H:%M %d:%m:%Y').txt
echo -e "$encrypted" > "/home/kali/Desktop/$filename"
