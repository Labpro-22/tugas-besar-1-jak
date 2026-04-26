# Nimonspoli 🎲

Implementasi permainan papan Nimonspoli berbasis CLI dalam bahasa C++17, dikembangkan sebagai Tugas Besar IF2010 Pemrograman Berorientasi Objek.

## Anggota Kelompok
| NIM | Nama |
|-----|------|
| 13524128  | Safira Berlianti |
| 13524131  | Amanda Aurellia Salsabilla |
| 13524134 | Salman Faiz Assidqi |
| 13524137 | Reysha Syafitri Mulya Ramadhan |
| 13524138  | Ahmad Rinofaros Muchtar |

## Cara Kompilasi dan Menjalankan

```bash
make all    # kompilasi
make run    # jalankan program
make clean  # bersihkan build
```

## Cara Bermain

Saat program dijalankan, pilih opsi:
- `1` → New Game
- `2` → Load Game

### Daftar Perintah

| Perintah | Deskripsi |
|----------|-----------|
| `LEMPAR_DADU` | Melempar dadu secara random |
| `ATUR_DADU X Y` | Mengatur nilai dadu secara manual |
| `AKHIRI_GILIRAN` | Mengakhiri giliran |
| `CETAK_PAPAN` | Menampilkan papan permainan |
| `CETAK_STATUS` | Menampilkan status pemain |
| `CETAK_PROPERTI` | Menampilkan daftar properti yang dimiliki |
| `CETAK_AKTA` | Menampilkan akta kepemilikan properti |
| `CETAK_LOG [n]` | Menampilkan log transaksi (opsional: n entri terakhir) |
| `GADAI` | Menggadaikan properti ke Bank |
| `TEBUS` | Menebus properti yang digadaikan |
| `BANGUN` | Membangun rumah atau hotel |
| `TAWAR <jumlah>` | Mengajukan tawaran saat lelang |
| `LEPAS` | Melewati giliran lelang |
| `GUNAKAN_KEMAMPUAN <indeks>` | Menggunakan kartu kemampuan |
| `BAYAR_DENDA` | Membayar denda untuk keluar penjara |
| `SIMPAN <nama_file>` | Menyimpan state permainan |
| `BANTUAN` | Menampilkan daftar perintah |
| `QUIT` | Keluar dari permainan |

## Struktur Proyek
    tugas-besar-1-jak/
    ├── config/         # File konfigurasi permainan
    ├── docs/           # Laporan
    ├── include/        # Header files
    │   ├── core/       # Game, AuctionManager, BankruptcyManager
    │   ├── models/     # Tile, Player, Card, dll
    │   ├── utils/      # ConfigLoader, SaveLoadManager, dll
    │   └── views/      # CLIRenderer, Command
    ├── src/            # Source files
    │   ├── core/
    │   ├── models/
    │   ├── utils/
    │   └── views/
    ├── makefile 
    └── README.md
    
## Fitur

- Papan permainan 40 petak dengan colored print sesuai color group
- Mekanisme kepemilikan properti (Street, Railroad, Utility)
- Sistem bangunan dengan aturan pemerataan
- Sistem lelang otomatis
- Kartu Kesempatan dan Dana Umum
- Kartu Kemampuan Spesial (MoveCard, ShieldCard, DiscountCard, TeleportCard, LassoCard, DemolitionCard)
- Sistem pajak (PPH dan PBM)
- Efek Festival
- Mekanisme penjara
- Sistem gadai dan tebus properti
- Save/Load permainan
- Transaction Logger
- Kondisi menang (Max Turn dan Bankruptcy)

## Bonus

- **BANTUAN** — command tambahan untuk menampilkan daftar perintah lengkap
- **Colored CLI** — tampilan terminal berwarna menggunakan ANSI escape codes