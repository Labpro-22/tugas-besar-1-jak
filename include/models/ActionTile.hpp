#pragma once

#include "Tile.hpp"

// Merepresentasikan petak aksi yang tidak bisa dimiliki
// tetapi memiliki efek tertentu saat diinjak
class ActionTile : public Tile
{
public:
    // ctor
    ActionTile(int idx, std::string cd, std::string nm);
};

// Merepresentasikan petak aksi untuk mengambil kartu sesuai tipe dek
class CardTile : public ActionTile
{
private:
    // tipe dek untuk diambil kartunya
    DeckType deckType;

public:
    // ctor
    CardTile(int idx, std::string cd, std::string nm, DeckType dt);
    // dipanggil saat mendarat, player mengambil kartu teratas sesuai tipe dek
    void onLanded(Player &player, Game &game) override;
};

// Merepresentasikan petak aksi untuk menerapkan festival pada properti yang dipilih
class FestivalTile : public ActionTile
{
public:
    // ctor
    FestivalTile(int idx, std::string cd, std::string nm);
    // dipanggil saat mendarat, memilih streetTile untuk menerapkan efek festival
    void onLanded(Player &player, Game &game) override;
};

// Merepresentaasikan petak aksi untuk membayar pajak sesuai tipe
class TaxTile : public ActionTile
{
private:
    // tipe pajak
    TaxType taxType;
    // jumlah flat pajak sesuai config
    int flatAmount;
    // persentase kekayaan (untuk tipe PPH) sesuai config
    double percentage;

public:
    // ctor
    TaxTile(int idx, std::string cd, std::string nm, TaxType txt, int fm, double pct = 0.0);
    // dipanggil saat mendarat, player membayar pajak sesuai tipe (untuk PPH ada dua opsi:
    // persen kekayaan atau flat)
    void onLanded(Player &player, Game &game) override;
};