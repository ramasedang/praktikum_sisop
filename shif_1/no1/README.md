# NO - 1

Bocchi hendak melakukan University Admission Test di Jepang. Bocchi ingin masuk ke universitas yang bagus. Akan tetapi, dia masih bingung sehingga ia memerlukan beberapa strategi untuk melakukan hal tersebut. Untung saja Bocchi menemukan file .csv yang berisi ranking universitas dunia untuk melakukan penataan strategi  : 
Bocchi ingin masuk ke universitas yang bagus di Jepang. Oleh karena itu, Bocchi perlu melakukan survey terlebih dahulu. Tampilkan 5 Universitas dengan ranking tertinggi di Jepang.
```
grep "Japan" list_univ.csv | sort -n -t , -k 1 | head -n 5 | awk -F "," '{print $1","$2","$3}'
```

Karena Bocchi kurang percaya diri, coba cari Faculty Student Score(fsr score) yang paling rendah diantara 5 Universitas dari hasil filter poin a.
```
grep -e "Japan" list_univ.csv | sort  -n -t , -k 1 | head -n 5 | sort -t , -k 9 | awk -F "," '{print $2","$9}'
```
Karena Bocchi takut salah membaca ketika memasukkan nama universitas, cari 10 Universitas di Jepang dengan Employment Outcome Rank(ger rank) paling tinggi.
```
grep "Japan" list_univ.csv | sort -n -t , -k 20 | head -n 10 | awk -F "," '{print $2","$20}'
```
Bocchi ngefans berat dengan universitas paling keren di dunia. Bantu bocchi mencari universitas tersebut dengan kata kunci keren.
```
grep -i "keren" list_univ.csv | awk -F "," '{print $1","$2","$3}'
```
