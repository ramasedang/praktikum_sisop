#!/bin/bash

# Get the current hour
HOUR=$(date +%H)

# Set the download interval to 10 hours
INTERVAL=10

# Check if the current hour is 0 (midnight)
if [ "$HOUR" -eq 0 ]; then
    # If it is, set the number of downloads to 1
    DOWNLOADS=1
else
    # Otherwise, set the number of downloads to the current hour
    DOWNLOADS="$HOUR"
fi

# Create a new directory for the downloaded files

if [ "$1" = "download" ]; then
    DIRNAME="kumpulan_$(($(ls -d kumpulan_* | wc -l) + 1))"
    mkdir "$DIRNAME"
    # Download the files
    for ((i = 1; i <= DOWNLOADS; i++)); do
        FILENAME="perjalanan_$i"
        wget "https://id.wikipedia.org/wiki/Soekarno#/media/Berkas:Presiden_Sukarno.jpg" -O "$DIRNAME/$FILENAME"
    done
elif [ "$1" = "zip" ]; then
    #!/bin/bash

# Get the current date and time
DATETIME=$(date +"%Y-%m-%d_%H-%M-%S")

DEVIL_ZIP_COUNT=$(ls -l | grep -c "devil_[0-9]*\.zip")

NEXT_DEVIL_ZIP_NAME="devil_$((DEVIL_ZIP_COUNT + 1)).zip"


# Find all directories with names starting with "kumpulan_"
DIRECTORIES=$(find . -type d -name "kumpulan_*")

# Zip all directories and move the zip file to the backup directory
zip -r $NEXT_DEVIL_ZIP_NAME $DIRECTORIES
else
    echo "Usage: $0 [download|zip]"
    exit 1
fi
