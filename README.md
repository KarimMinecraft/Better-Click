# Better Click

Mod Geode untuk Geometry Dash (Android).

- Suara **click** saat layar ditekan
- Suara **release** kecil saat jari dilepas
- Volume, variasi pitch, dan file suara bisa dikustomisasi
- Mendukung clickpack ZCB3
- Tombol tes di menu utama: Click, Release, Reload, Setting

## Cara build lewat HP (GitHub Actions)

1. Buat repo baru di GitHub, lalu upload semua isi folder ini (termasuk folder `.github`).
2. Buka tab **Actions**, lalu tunggu workflow *Build Geode Mod* selesai (atau jalankan manual lewat *Run workflow*).
3. Buka run yang sudah hijau, lalu unduh artifact **Build Output**.
4. Ekstrak zip-nya, ambil file `user.better-click.geode`.
5. Taruh file `.geode` di folder mods Geode:
   `Android/data/com.geode.launcher/files/geode/mods/`
   (atau impor lewat menu Geode di dalam game).

## Cara pakai clickpack ZCB3

Setelah game dijalankan sekali dengan mod ini, folder berikut dibuat otomatis:

`Android/data/com.geode.launcher/files/geode/config/user.better-click/clickpack/`

Salin isi clickpack ZCB3 ke dalam folder itu. Aturannya:

- File `.wav`, `.mp3`, `.ogg` di semua subfolder dipindai.
- Jika nama folder atau file mengandung kata `release`, dipakai sebagai suara release.
- Selain itu dipakai sebagai suara click.

Setelah menyalin file, tekan tombol **Reload** di menu utama.

## Suara custom

Di pengaturan mod, isi **File suara click custom** / **File suara release custom** dengan path lengkap file suara. Kalau kosong, dipakai clickpack.
