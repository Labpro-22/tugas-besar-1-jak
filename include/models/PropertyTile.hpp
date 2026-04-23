#pragma once

#include "Tile.hpp"

// Merepresentasikan petak properti yang bisa dimiliki pemain dan mempunyai nilai sewa
class PropertyTile : public Tile
{
protected:
    int buyPrice;          // harga beli properti
    int mortgageValue;     // nilai uang yang diterima saat properti digadai
    Player *owner;         // pointer ke player pemilik properti
    PropertyStatus status; // status properti (bank, owned, mortgaged)
public:
    // constructor
    PropertyTile(int idx, std::string cd, std::string nm, int bp, int mv);
    // menghitung biaya sewa sesuai jenis properti (pure virtual)
    virtual int calculateRent(int diceTotal) = 0;
    // menggadai properti ke bank
    void mortgage();
    // menebus properti dari bank
    void redeem();
    // getter pemilik properti
    Player *getOwner();
    // cek apakah properti sedang digadai
    bool isMortgaged();

    int getPrice();

    void PropertyTile::changeOwner(Player *newOwner);
};

// Merepresentasikan petak properti yang dapat ditingkatkan dan dikelompokkan berdasarkan color group
class StreetTile : public PropertyTile
{
private:
    std::string colorGroup; // kelompok warna
    std::vector<int> rents; // tabel sewa level 0 - 5 sesuai file config
    int houseCost;          // biaya membangun 1 rumah
    int hotelCost;          // biaya upgrade ke level 5/hotel
    int buildingLevel;      // level bangunan saat ini (0 - 5)
    int festivalMultiplier; // pengali sewa dari efek festival (no efek: 1, efek: 2, 4, 8 maks)
    int festivalDuration;   // durasi efek festival (0 - 3 giliran)
public:
    // ctor
    StreetTile(int idx, std::string cd, std::string nm, int bp, int mv, std::string cg,
               std::vector<int> rnt, int hc, int htc);
    // hitung biaya sewa berdasarkan level bangunan, ismonopolize(), dan efek festival
    int calculateRent(int diceTotal) override;
    // dipanggil ketika pemain mendarat di petak
    void onLanded(Player &player, Game &game) override;
    // menaikkan level bangunan dan memotong saldo pemain
    void build();
    // menjual bangunan ke Bank seharga 1/2 harga beli
    void sell();
    // cek apakah pemilik menguasai color group
    bool isMonopolized();
    // cek syarat pemerataan bangunan antar petak color group
    bool canBuild();
    // mengaktifkan efek festival yang menggandakan nilai sewa
    void applyFestival();
    // mengurangi durasi festival tiap giliran
    void tickFestival();
    // cetak akta kepemilikan, menggunakan CLI renderer
    void printDeed();
};

// Merepresentasikan petak properti berupa stasiun
class RailroadTile : public PropertyTile
{
private:
    std::map<int, int> rentTable; // pemetaan jumlah railload yang dimiliki ke biaya sewa
public:
    // ctor
    RailroadTile(int idx, std::string cd, std::string nm, int bp, int mv,
                 std::map<int, int> rt);
    // menghitung sewa berdasarkan jumlah railroad pemilik
    int calculateRent(int diceTotal) override;
    // dipanggil saat pemain mendarat, lengsung membberikan kepemilikan jika belum ada pemilik
    void onLanded(Player &player, Game &game) override;
};

// Merepresentasikan petak properti berupa utilitas (PLN dan PAM)
class UtilityTile : public PropertyTile
{
private:
    std::map<int, int> multiplierTable; // pemetaan jumlah utility yang dimiliki ke faktor pengali sewa
public:
    // constructor
    UtilityTile(int idx, std::string cd, std::string nm, int bp, int mv,
                std::map<int, int> mt);
    // menghitung biaya sewa berdasarkan diceTotal x faktor pengali
    int calculateRent(int diceTotal) override;
    // dipanggil saat pemain mendarat, lengsung membberikan kepemilikan jika belum ada pemilik
    void onLanded(Player &player, Game &game) override;
};