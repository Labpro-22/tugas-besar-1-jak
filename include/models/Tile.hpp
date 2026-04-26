#pragma once

#include <string>
#include <vector>
#include <map>

// Forward declarations
class Player;
class Game;

// Status petak properti
enum class PropertyStatus {
    BANK, OWNED, MORTGAGED
};

// Tipe dek kartu pada game
enum class DeckType {
    CHANCE, GENERAL_FUNDS
};

// Tipe pajak pada petak pajak
enum class TaxType {
    PPH, PBM
};

// Merepresentasikan petak-petak pada papan permainan Nimonspoli
class Tile {
    protected:
        int index; // posisi tile dalam board
        std::string code; // kode unik
        std::string name; // nama
    public:
        // constructor
        Tile(int idx, std::string cd, std::string nm);
        // destructor
        virtual ~Tile() = default;
        // dipanggil saat player mendarat
        virtual void onLanded(Player& player, Game& game) {};
        // mendapatkan kode warna untuk tampilan board
        virtual std::string getDisplayColor();
        // getter atribut code
        std::string getCode() const;
        // getter atribut index
        int getIndex() const;
        // getter atribut nama
        std::string getName() const;
};