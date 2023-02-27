#!/bin/bash

# Membuat direktori users dan file users.txt
mkdir users
touch users/users.txt

# Fungsi untuk memvalidasi password
is_valid_password() {
    local PASSWORD=$1
    local USERNAME=$2

    # Validasi minimal 8 karakter dan alphanumeric
    if ! [[ $PASSWORD =~ ^[[:alnum:]]{8,}$ ]]; then
        echo "Password harus terdiri dari minimal 8 karakter dan alphanumeric"
        return 1
    fi

    # Validasi minimal 1 huruf kapital dan 1 huruf kecil
    if ! [[ $PASSWORD =~ [A-Z] ]] || ! [[ $PASSWORD =~ [a-z] ]]; then
        echo "Password harus memiliki minimal 1 huruf kapital dan 1 huruf kecil"
        return 1
    fi

    # Validasi tidak sama dengan username
    if [[ $PASSWORD == $USERNAME ]]; then
        echo "Password tidak boleh sama dengan username"
        return 1
    fi

    # Validasi tidak boleh menggunakan kata chicken atau ernie
    if [[ $PASSWORD =~ chicken|ernie ]]; then
        echo "Password tidak boleh menggunakan kata chicken atau ernie"
        return 1
    fi

    # Jika password lolos semua validasi, return 0
    return 0
}

# Fungsi untuk mencatat log
log() {
    local MESSAGE=$1
    echo "$(date +"%y/%m/%d %H:%M:%S") $MESSAGE" >> log.txt
}

# Fungsi untuk registrasi user
register() {
    local USERNAME=$1
    local PASSWORD=""

    # Loop untuk input password yang valid
    while true; do
        read -s -p "Masukkan password untuk $USERNAME: " PASSWORD
        echo
        if is_valid_password $PASSWORD $USERNAME; then
            break
        fi
    done


    # Cek apakah username sudah terdaftar
    if grep -q "^$USERNAME:" users/users.txt; then
        log "REGISTER: ERROR User $USERNAME already exists"
        echo "Username sudah terdaftar"
    else
        # Simpan data user ke file
        echo "$USERNAME:$PASSWORD" >> users/users.txt
        log "REGISTER: INFO User $USERNAME registered successfully"
        echo "Registrasi berhasil dilakukan untuk $USERNAME"
    fi
}

# Fungsi untuk login user
login() {
    local USERNAME=$1
    local PASSWORD=""

    # Loop untuk input password yang valid
    while true; do
        read -s -p "Masukkan password untuk $USERNAME: " PASSWORD
        echo
        if is_valid_password $PASSWORD $USERNAME; then
            break
        fi
    done 

    # Cek apakah username dan password sudah terdaftar di file
    if grep -q "^$USERNAME:$PASSWORD$" users/users.txt; then
        log "LOGIN: INFO User $USERNAME logged in"
        echo "Login berhasil dilakukan untuk $USERNAME"
    else
        log "LOGIN: ERROR Failed login attempt on user $USERNAME"
        echo "Username atau password salah"
    fi
}

# Menu utama
while true; do
    echo "Silakan pilih opsi:"
    echo "1. Registrasi"
    echo "2. Login"
    read -p "Pilihan anda: " CHOICE

    case $CHOICE in
        1)
            read -p "Masukkan username baru: " USERNAME
            register $USERNAME
            ;;
       2)
        read -p "Masukkan username: " USERNAME
        login $USERNAME
        ;;
    *)
        echo "Pilihan tidak valid"
        ;;
esac

done