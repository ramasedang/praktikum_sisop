#!/bin/bash

filename="$1"
jam="${filename:0:2}"

encrypted=$(cat "$filename")

decrypted=""
for ((i=0; i<${#encrypted}; i++)); do
    char="${encrypted:$i:1}"
    if [[ "$char" =~ [A-Za-z] ]]; then
        if [[ "$char" =~ [A-Z] ]]; then
            charcode=$(( ( $(printf '%d' "'$char") - 65 - $jam + 26) % 26 + 65 ))
            char=$(printf \\$(printf '%03o' "$charcode"))
        else
            charcode=$(( ( $(printf '%d' "'$char") - 97 - $jam + 26) % 26 + 97 ))
            char=$(printf \\$(printf '%03o' "$charcode"))
        fi
    fi
    decrypted="${decrypted}${char}"
done

decrypted_filename="${filename%.*}_decrypted.txt"
echo -e "$decrypted" > "$decrypted_filename"