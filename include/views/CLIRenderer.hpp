#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <sstream>

// Forward declarations
class Board;
class Player;
class PropertyTile;
class StreetTile;
class RailroadTile;
class UtilityTile;
class Tile;

class CLIRenderer {
    private:
    // Lebar tiap sel papan
    static const int CELL_WIDTH = 10;
    // Jumlah petak per sisi
    static const int SIDE_SIZE = 11;

    // ===== Helper Board Rendering =====
    void renderTopRow(const Board& board, const std::vector<Player*>& players) const;
    void renderBottomRow(const Board& board, const std::vector<Player*>& players) const;
    void renderMiddleRows(const Board& board, const std::vector<Player*>& players, int currentTurn, int maxTurn, int currentPlayerIndex) const;

    // Kumpulkan indeks pemain yang ada di tile tertentu
    std::vector<int> getPlayersAtTile(int tileIndex, const std::vector<Player*>& players) const;

    // Format simbol pemain: "P1 ^" / "P2(1)"
    std::string formatPlayerSymbols(const std::vector<int>& playerIndices, const std::vector<Player*>& players) const;

    // Format baris separator "+----------+..."
    std::string makeSeparatorRow(int numCells) const;

    // Pad string ke lebar tertentu
    static std::string padTo(const std::string& s, int width);

    // Hitung panjang visible string
    static int visibleLength(const std::string& s);

    // Render panel tengah papan (legenda + turn info)
    std::string getCenterLine(int row, int currentTurn, int maxTurn, int currentPlayerIndex, const std::vector<Player*>& players) const;

    public:
    CLIRenderer() {}
    ~CLIRenderer() {}

    // ===== ANSI Color Codes =====
    // [CK]=Coklat  [MR]=Merah   [BM]=Biru Muda  [KN]=Kuning
    // [PK]=Pink    [HJ]=Hijau   [OR]=Orange     [BT]=Biru Tua
    // [DF]=Default [AB]=Utilitas
    static const std::string RESET;
    static const std::string BOLD;
    static const std::string COLOR_COKLAT;
    static const std::string COLOR_BIRU_MUDA;
    static const std::string COLOR_MERAH_MUDA;
    static const std::string COLOR_ORANGE;
    static const std::string COLOR_MERAH;
    static const std::string COLOR_KUNING;
    static const std::string COLOR_HIJAU;
    static const std::string COLOR_BIRU_TUA;
    static const std::string COLOR_DEFAULT;

    // ===== CETAK_PAPAN =====
    void renderBoard(const Board& board, const std::vector<Player*>& players, int currentTurn, int maxTurn, int currentPlayerIndex) const;

    // ===== CETAK_AKTA =====
    // Menampilkan akta kepemilikan sebuah properti street
    // Jika tile bukan properti atau tidak ditemukan, tampilkan pesan error
    void printDeed(const StreetTile& tile) const;
    void printDeed(const RailroadTile& tile) const;
    void printDeed(const UtilityTile& tile) const;
    void printDeedNotFound(const std::string& tileCode) const;
    
    // ===== CETAK_PROPERTI =====
    // Menampilkan semua properti milik pemain, dikelompokkan per warna
    void printPropertyInventory(const Player& player) const;

    // ===== CETAK_STATUS =====
    // Menampilkan status pemain saat ini (saldo, posisi, status)
    void printPlayerStatus(const Player& player, const Board& board) const;

    // ===== Menu dan Header =====
    void printMainMenu() const;
    void printTurnHeader(const Player& player, int turn, int maxTurn) const;
    void printPrompt() const;
    void printHelp() const;

    // ===== Pesan Umum =====
    void printError(const std::string& message) const;
    void printInfo(const std::string& message) const;
    void printSuccess(const std::string& message) const;
    void printWinner(const Player& winner, int turn) const;

    // ===== Helper Warna =====
    static std::string getColorCode(const std::string& colorGroup);
    static std::string getColorTag(const std::string& colorGroup);
};