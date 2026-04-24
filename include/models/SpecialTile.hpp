#pragma once

#include "Tile.hpp"

// Merepresentasikan petak spesial yang terletak di 4 sudut papan
class SpecialTile : public Tile {
    public:
        // ctor
        SpecialTile(int idx, std::string cd, std::string nm);
        // getter tile type (return SEPCIAL)
        TileType getTileType() const override;
};

// Petak spesial berupa petak mulai
class GoTile : public SpecialTile {
    private:
        // gaji yang diterima sesuai config
        int salary;
    public:
        // ctor
        GoTile(int idx, std::string cd, std::string nm, int slr);
        // dipanggil saat mendarat atau lewat, player mendapatkan gaji
        void onLanded(Player& player, Game& game) override;
};

// Petak spesial berupa petak penjara
class JailTile : public SpecialTile {
    private:
        // denda keluar pajak sesuai config
        int fine;
    public:
        // ctor
        JailTile(int idx, std::string cd, std::string nm, int fn);
        // dipanggil saat mendarat, menentukan apakah player tahanan atau angin lalu
        void onLanded(Player& player, Game& game) override;
        // memproses giliran pemain yang sedang ditahan
        void processJailTurn(Player& player, Game& game);
};

// Petak spesial untuk pergi ke penjara
class GoToJailTile : public SpecialTile {
    public:
        // ctor
        GoToJailTile(int idx, std::string cd, std::string nm);
        // dipanggil saat mendarat, memindahkan player ke petak penjara dan mengubah status pemain tsb
        void onLanded(Player& player, Game& game) override;
};

// Petak spesial untuk istirahat (do nothing)
class FreeParkingTile : public SpecialTile {
    public:
        // ctor
        FreeParkingTile(int idx, std::string cd, std::string nm);
        // dipanggil saat mendarat, do nothing
        void onLanded(Player& player, Game& game) override;
};