#!/bin/bash

HOUR=$(date +%H)

INTERVAL=10

if [ "$HOUR" -eq 0 ]; then
    # Kalau sedang jam 00:00, set download menjadi 1.
    DOWNLOADS=1
else
    
    DOWNLOADS="$HOUR"
fi

if [ "$1" = "download" ]; then
    DIRNAME="kumpulan_$(($(ls -d kumpulan_* | wc -l) + 1))"
    mkdir "$DIRNAME"

    for ((i = 1; i <= DOWNLOADS; i++)); do
        FILENAME="perjalanan_$i"
        wget "https://id.wikipedia.org/wiki/Soekarno#/media/Berkas:Presiden_Sukarno.jpg" -O "$DIRNAME/$FILENAME"
    done
elif [ "$1" = "zip" ]; then
    #!/bin/bash

DATETIME=$(date +"%Y-%m-%d_%H-%M-%S")

DEVIL_ZIP_COUNT=$(ls -l | grep -c "devil_[0-9]*\.zip")

NEXT_DEVIL_ZIP_NAME="devil_$((DEVIL_ZIP_COUNT + 1)).zip"

DIRECTORIES=$(find . -type d -name "kumpulan_*")

zip -r $NEXT_DEVIL_ZIP_NAME $DIRECTORIES
else
    echo "Usage: $0 [download|zip]"
    exit 1
fi

# 0 13,23 * * * home/reyhan/Documents/shift1/kobeni_liburan.sh download
# 0 0 * * * home/reyhan/Documents/shift1/kobeni_liburan.sh zip
