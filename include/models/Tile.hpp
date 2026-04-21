#pragma once

#include <string>
#include <vector>
#include <map>

// Forward declaration agar Tile mengenali kelas Player dan Game
class Player;
class Game;

// Status petak properti
enum class PropertyStatus {
    BANK, OWNED, MORTGAGED
};

// Merepresentasikan petak-petak pada papan permainan Nimonspoli
class Tile {
    protected:
        int index; // posisi tile dalam board
        std::string code; // kode unik
        std::string name; // nama
    public:
        // constructor
        Tile(int idx, std::string cd, std::string nm)
            : index(idx), code(cd), name(nm) {}
        // dipanggil saat player mendarat
        virtual void onLanded(Player& player, Game& game) = 0;
        // mendapatkan kode warna untuk tampilan board
        virtual std::string getDisplayColor() {return "WHITE";}
        // getter atribut code
        std::string getCode() const { return code; }
        // getter atribut index
        int getIndex() const { return index; }
        virtual ~Tile() = default;
};

