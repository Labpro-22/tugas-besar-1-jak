#include "ConfigLoader.hpp"
#include "PropertyTile.hpp"
#include "ActionTile.hpp"
#include "SpecialTile.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

// Ngeload papan permainan
std::vector<Tile*> ConfigLoader::loadBoard(const std::string& folderPath) {
    // Inisialisasi array 40 petak kosong
    std::vector<Tile*> board(40, nullptr);

    // Load tabel sewa railroad dan utility (butuh buat bikin tile)
    std::map<int, int> railroadTable = loadRailroad(folderPath);
    std::map<int, int> utilityTable = loadUtility(folderPath);
    std::map<std::string, int> specialConfig = loadSpecial(folderPath);
    std::map<std::string, int> taxConfig = loadTax(folderPath);

    int goSalary = 200;
    if (specialConfig.count("GO_SALARY")) {
        goSalary = specialConfig["GO_SALARY"];
    }

    int jailFine = 50;
    if (specialConfig.count("JAIL_FINE")) {
        jailFine = specialConfig["JAIL_FINE"];
    }

    int pphFlat = 200;
    if (taxConfig.count("PPH_FLAT")) {
        pphFlat = taxConfig["PPH_FLAT"];
    }

    int pphPersen = 10;
    if (taxConfig.count("PPH_PERSENTASE")) {
        pphPersen = taxConfig["PPH_PERSENTASE"];
    }

    int pbmFlat = 100;
    if (taxConfig.count("PBM_FLAT")) {
        pbmFlat = taxConfig["PBM_FLAT"];
    }

    // ===== property.txt =====
    std::ifstream fileProperty(folderPath + "property.txt");
    if (!fileProperty.is_open()) {
        std::cerr << "GAGAL DIBUKA: " << folderPath << "property.txt\n";
    } else {
        std::string line;
        while (std::getline(fileProperty, line)) {
            if (line.empty() || line.find("ID KODE") != std::string::npos) {
                continue; // Skip baris kosong atau header
            }
            
            std::stringstream ss(line);
            int id;
            std::string kode, buffer, jenis, warna;
            
            
            // Baca data berurutan sesuai format txt
            ss >> id >> kode;
            std::string nama = "";
            // Loop baca kata perkata sampai nemu jenis petaknya
            while (ss >> buffer) {
                if (buffer == "STREET" || buffer == "RAILROAD" || buffer == "UTILITY") {
                    jenis = buffer;
                    break;
                }
                // Kalau bukan jenis petak, berarti masih bagian dari nama
                if (!nama.empty()) {
                    nama = nama + " "; // Tambah spasi di antara kata
                }
                nama = nama + buffer;
            }

            // Ganti underscore jadi spasi di nama
            std::replace(nama.begin(), nama.end(), '_', ' ');

            if (id < 1 || id > 40) {
                continue;
            }
            int arrIdx = id - 1;

            if (jenis == "STREET") {
                int hargaLahan, nilaiGadai, upgradeRumah, upgradeHotel;
                ss >> warna >> hargaLahan >> nilaiGadai >> upgradeRumah >> upgradeHotel;
 
                // Baca sewa level 0-5
                std::vector<int> rents;
                int sewa;
                while (ss >> sewa) {
                    rents.push_back(sewa);
                }

                // Pastikan ada 6 level sewa
                while ((int)rents.size() < 6) {
                    rents.push_back(0);
                }

                board[arrIdx] = new StreetTile(id, kode, nama, hargaLahan, nilaiGadai, warna, rents, upgradeRumah, upgradeHotel);
            } else if (jenis == "RAILROAD") {
                int nilaiGadai;
                ss >> warna >> nilaiGadai;

                // Harga beli railroad = 0 sesuai spesifikasi (kepemilikan otomatis)
                board[arrIdx] = new RailroadTile(id, kode, nama, 0, nilaiGadai, railroadTable);
            } else if (jenis == "UTILITY") {
                int nilaiGadai;
                ss >> warna >> nilaiGadai;
                board[arrIdx] = new UtilityTile(id, kode, nama, 0, nilaiGadai, utilityTable);
            }
        }
        fileProperty.close();
    }

    // ===== aksi.txt =====
    std::ifstream fileAksi(folderPath + "aksi.txt");
    if (!fileAksi.is_open()) {
        std::cerr << "GAGAL DIBUKA: " << folderPath << "aksi.txt\n";
    } else {
        std::string line;
        while (std::getline(fileAksi, line)) {
            if (line.empty() || line.find("ID KODE") != std::string::npos) {
                continue;
            }

            std::stringstream ss(line);
            int id;
            std::string kode, nama, jenis, warna;

            // Baca datanya
            ss >> id >> kode >> nama >> jenis >> warna;

            // Underscore jadi spasi
            std::replace(nama.begin(), nama.end(), '_', ' ');

            if (id < 1 || id > 40) {
                continue;
            }
            int arrIdx = id - 1;

            if (jenis == "SPESIAL") {
                if (kode == "GO") {
                    board[arrIdx] = new GoTile(id, kode, nama, goSalary);
                } else if (kode == "PEN") {
                    board[arrIdx] = new JailTile(id, kode, nama, jailFine);
                } else if (kode == "BBP") {
                    board[arrIdx] = new FreeParkingTile(id, kode, nama);
                } else if (kode == "PPJ") {
                    board[arrIdx] = new GoToJailTile(id, kode, nama);
                }
            } else if (jenis == "KARTU") {
                // Tentukan tipe dek dari kode
                DeckType tipe;
                if (kode == "KSP") {
                    tipe = DeckType::CHANCE;
                } else {
                    tipe = DeckType::GENERAL_FUNDS;
                }
                board[arrIdx] = new CardTile(id, kode, nama, tipe);
            } else if (jenis == "FESTIVAL") {
                board[arrIdx] = new FestivalTile(id, kode, nama);
            } else if (jenis == "PAJAK") {
                if (kode == "PPH") {
                    board[arrIdx] = new TaxTile(id, kode, nama, TaxType::PPH, pphFlat, pphPersen / 100.0);
                } else {
                    board[arrIdx] = new TaxTile(id, kode, nama, TaxType::PBM, pbmFlat);
                }
            }
        }
        fileAksi.close();
    }

    // Cek apakah ada tile yang masih nullptr (berarti data config tidak lengkap)
    for (int i = 0; i < 40; i++) {
        if (board[i] == nullptr) {
            std::cerr << "[WARNING] Tile indeks " << (i + 1) << " tidak berhasil diload.\n";
        }
    }
    return board;
}

// Ngeload konfigurasi angka
std::map<std::string, int> ConfigLoader::loadMisc(const std::string& folderPath) {
    std::map<std::string, int> config;
    std::ifstream file(folderPath + "misc.txt");
    std::string line;
    if (file.is_open() && std::getline(file, line) && std::getline(file, line)) {
        std::stringstream ss(line);
        int maxTurn, saldoAwal;
        ss >> maxTurn >> saldoAwal;
        config["MAX_TURN"] = maxTurn;
        config["SALDO_AWAL"] = saldoAwal;
    }
    return config;
}

std::map<std::string, int> ConfigLoader::loadTax(const std::string& folderPath) {
    std::map<std::string, int> config;
    std::ifstream file(folderPath + "tax.txt");
    std::string line;
    if (file.is_open() && std::getline(file, line) && std::getline(file, line)) {
        std::stringstream ss(line);
        int pphFlat, pphPersen, pbmFlat;
        ss >> pphFlat >> pphPersen >> pbmFlat;
        config["PPH_FLAT"] = pphFlat;
        config["PPH_PERSENTASE"] = pphPersen;
        config["PBM_FLAT"] = pbmFlat;
    }
    return config;
}

std::map<std::string, int> ConfigLoader::loadSpecial(const std::string& folderPath) {
    std::map<std::string, int> config;
    std::ifstream file(folderPath + "special.txt");
    std::string line;
    if (file.is_open() && std::getline(file, line) && std::getline(file, line)) {
        std::stringstream ss(line);
        int goSalary, jailFine;
        ss >> goSalary >> jailFine;
        config["GO_SALARY"] = goSalary;
        config["JAIL_FINE"] = jailFine;
    }
    return config;
}

// Ngeload tabel sewa
std::map<int, int> ConfigLoader::loadRailroad(const std::string& folderPath) {
    std::map<int, int> table;
    std::ifstream file(folderPath + "railroad.txt");
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line.find("JUMLAH") != std::string::npos) {
            continue;
        } 
        std::stringstream ss(line);
        int jumlah, sewa;
        ss >> jumlah >> sewa;
        table[jumlah] = sewa;
    }
    return table;
}

std::map<int, int> ConfigLoader::loadUtility(const std::string& folderPath) {
    std::map<int, int> table;
    std::ifstream file(folderPath + "utility.txt");
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line.find("JUMLAH") != std::string::npos) {
            continue;
        }
        std::stringstream ss(line);
        int jumlah, pengali;
        ss >> jumlah >> pengali;
        table[jumlah] = pengali;
    }
    return table;
}