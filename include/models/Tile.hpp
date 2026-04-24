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

// Tipe tile
enum class TileType {
    PROPERTY, ACTION, SPECIAL, UNDEFINED
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
        // dipanggil saat player mendarat
        virtual void onLanded(Player& player, Game& game) = 0;
        // mendapatkan kode warna untuk tampilan board
        virtual std::string getDisplayColor();
        // getter atribut code
        std::string getCode() const;
        // getter atribut index
        int getIndex() const;
        // getter atribut nama
        std::string getName() const;
        // getter tile type
        virtual TileType getTileType() const;
        virtual ~Tile() = default;
};