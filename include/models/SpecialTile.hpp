#pragma once

#include "Tile.hpp"

// Merepresentasikan petak spesial yang terletak di 4 sudut papan
class SpecialTile : public Tile {
    public:
        // ctor
        SpecialTile(int idx, std::string cd, std::string nm);
};

// Petak spesial berupa petak mulai
class GoTile : public SpecialTile {
    private:
        // gaji yang diterima sesuai config
        int salary;
    public:
        // ctor
        GoTile(int idx, std::string cd, std::string nm, int slr);
};

// Petak spesial berupa petak penjara
class JailTile : public SpecialTile {
    private:
        // denda keluar pajak sesuai config
        int fine;
    public:
        // ctor
        JailTile(int idx, std::string cd, std::string nm, int fn);
        // memproses giliran pemain yang sedang ditahan
        void processJailTurn(Player& player, Game& game);
};

// Petak spesial untuk pergi ke penjara
class GoToJailTile : public SpecialTile {
    public:
        // ctor
        GoToJailTile(int idx, std::string cd, std::string nm);
};

// Petak spesial untuk istirahat (do nothing)
class FreeParkingTile : public SpecialTile {
    public:
        // ctor
        FreeParkingTile(int idx, std::string cd, std::string nm);
};