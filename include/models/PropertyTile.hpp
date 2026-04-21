#pragma once

#include "Tile.hpp"

// Merepresentasikan petak properti yang bisa dimiliki pemain dan mempunyai nilai sewa
class PropertyTile : public Tile {
    protected:
        int buyPrice; // harga beli properti
        int mortgageValue; // nilai uang yang diterima saat properti digadai
        Player* owner; // pointer ke player pemilik properti
        PropertyStatus status; // status properti (bank, owned, mortgaged)
    public:
        // constructor
        PropertyTile(int idx, std::string cd, std::string nm, int bp, int mv)
            : Tile(idx, cd, nm), buyPrice(bp), mortgageValue(mv), owner(nullptr), status(PropertyStatus::BANK) {}
        // menghitung biaya sewa sesuai jenis properti (pure virtual)
        virtual int calculateRent(int diceTotal) = 0;
        // menggadai properti ke bank
        void mortgage() {
            status = PropertyStatus::MORTGAGED;
        }
        // menebus properti dari bank
        void redeem() {
            status = PropertyStatus::OWNED;
        }
        // getter pemilik properti
        Player* getOwner() {
            return owner;
        }
        // cek apakah properti sedang digadai
        bool isMortgaged() { return status == PropertyStatus::MORTGAGED; }
};

// Merepresentasikan petak properti yang dapat ditingkatkan dan dikelompokkan berdasarkan color group
class StreetTile : public PropertyTile {
    private:
        std::string colorGroup; // kelompok warna
        std::vector<int> rents; // tabel sewa level 0 - 5 sesuai file config
        int houseCost; // biaya membangun 1 rumah
        int hotelCost; // biaya upgrade ke level 5/hotel
        int buildingLevel; // level bangunan saat ini (0 - 5)
        int festivalMultiplier; // pengali sewa dari efek festival (no efek: 1, efek: 2, 4, 8 maks)
        int festivalDuration; // durasi efek festival (0 - 3 giliran)
    public:
        // ctor
        StreetTile(int idx, std::string cd, std::string nm, int bp, int mv, std::string cg, 
            std::vector<int> rnt, int hc, int htc) : PropertyTile(idx, cd, nm, bp, mv), colorGroup(cg), rents(rnt),buildingLevel(0),
            festivalMultiplier(1), festivalDuration(0) {}
        // hitung biaya sewa berdasarkan level bangunan, ismonopolize(), dan efek festival
        int calculateRent(int diceTotal) override {
            if (status != PropertyStatus::OWNED) {
                return 0;
            }
            int baseRent = rents[buildingLevel];
            if (buildingLevel == 0 && isMonopolized()) {
                baseRent *= 2;
            }
            if (festivalDuration > 0) {
                baseRent *= festivalMultiplier;
            }
            return baseRent;
        }
        // dipanggil ketika pemain mendarat di petak
        void onLanded(Player& player, Game& game) override {}
        // menaikkan level bangunan dan memotong saldo pemain
        void build() {
            if (canBuild()) {
                if (buildingLevel < 5) {
                    buildingLevel++;
                }
            }
        }
        // menjual bangunan ke Bank seharga 1/2 harga beli
        void sell() {
            if (status != PropertyStatus::OWNED) {
                return;
            }
            int nilaiGadai = 0;
            if (buildingLevel > 0) {
                if (buildingLevel >= 5) {
                    nilaiGadai += (houseCost / 2) * 4 + (hotelCost / 2);
                } else {
                    nilaiGadai += (houseCost / 2) * buildingLevel;
                }
            }
            nilaiGadai += mortgageValue;
            mortgage();
        }
        // cek apakah pemilik menguasai color group
        bool isMonopolized();
        // cek syarat pemerataan bangunan antar petak color group
        bool canBuild();
        // mengaktifkan efek festival yang menggandakan nilai sewa
        void applyFestival() {
            if (festivalMultiplier < 8) {
                festivalMultiplier *= 2;
            }
            festivalDuration = 3;
        }
        // mengurangi durasi festival tiap giliran
        void tickFestival() {
            if (festivalDuration > 0) {
                festivalDuration--;
            }
            if (festivalDuration == 0) {
                festivalMultiplier = 1;
            }
        }
        // cetak akta kepemilikan, menggunakan CLI renderer
        void printDeed();
};

// Merepresentasikan petak properti berupa stasiun
class RailroadTile : public PropertyTile {
    private:
        std::map<int, int> rentTable; // pemetaan jumlah railload yang dimiliki ke biaya sewa
    public:
        // ctor
        RailroadTile(int idx, std::string cd, std::string nm, int bp, int mv, 
            std::map<int, int> rt) : PropertyTile(idx, cd, nm, bp, mv), rentTable(rt) {}
        // menghitung sewa berdasarkan jumlah railload pemilik
        int calculateRent(int diceTotal) override {
            if (status == PropertyStatus::OWNED) {
                int railloadCount = 0; // = jumlah railload player
                return rentTable[railloadCount];
            }
            return 0;
        }
        // dipanggil saat pemain mendarat, lengsung membberikan kepemilikan jika belum ada pemilik
        void onLanded(Player& player, Game& game) override {}
};

// Merepresentasikan petak properti berupa utilitas (PLN dan PAM)
class UtilityTile : public PropertyTile {
    private:
        std::map<int, int> multiplierTable; // pemetaan jumlah utility yang dimiliki ke faktor pengali sewa
    public:
        // constructor
        UtilityTile(int idx, std::string cd, std::string nm, int bp, int mv, 
            std::map<int, int> mt) : PropertyTile(idx, cd, nm, bp, mv), multiplierTable(mt) {}
        // menghitung biaya sewa berdasarkan diceTotal x faktor pengali
        int calculateRent(int diceTotal) override {
            if (status == PropertyStatus::OWNED) {
                int utilityCount = 0; // = jumlah utility player
                return multiplierTable[utilityCount] * diceTotal;
            }
            return 0;
        }
        // dipanggil saat pemain mendarat, lengsung membberikan kepemilikan jika belum ada pemilik
        void onLanded(Player& player, Game& game) override {}
};