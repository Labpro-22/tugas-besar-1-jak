#pragma once
#include <string>
#include <vector>
#include <fstream>

// Forward declarations
class Player;
class Board;
class TransactionLogger;
class SkillCard;
template <typename T> class CardDeck;
class SkillCard;

/*
 * Format file save
 * <TURN_SAAT_INI> <MAX_TURN>
 * <JUMLAH_PEMAIN>
 * [STATE_PEMAIN x N]
 * <URUTAN_GILIRAN>
 * <GILIRAN_AKTIF>
 * <STATE_PROPERTI>
 * <STATE_DECK>
 * <STATE_LOG>
 */
class SaveLoadManager {
    private:
    // ===== Save Helpers =====
    void savePlayerState(std::ofstream& out, const Player* player) const;
    void savePropertyState(std::ofstream& out, const Board& board) const;
    void saveDeckState(std::ofstream& out, const CardDeck<SkillCard>& skillDeck) const;
    void saveLogState(std::ofstream& out, const TransactionLogger& logger) const;

    // ===== Load Helpers =====
    Player* loadPlayerState(std::ifstream& in) const;
    void loadPropertyState(std::ifstream& in, Board& board, const std::vector<Player*>& players) const;
    std::vector<std::string> loadDeckState(std::ifstream& in) const;
    void loadLogState(std::ifstream& in, TransactionLogger& logger) const;

    // Buat SkillCard dari nama jenis dan nilai
    SkillCard* createSkillCard(const std::string& type, int value, int duration) const;

    public:
    SaveLoadManager() {}
    ~SaveLoadManager() {}

    bool saveGame(const std::string& filename, int turn, int maxTurn, const std::vector<Player*>& players, const std::vector<int>& turnOrder, int currentPlayerIndex, const Board& board, const CardDeck<SkillCard>& skillDeck, const TransactionLogger& logger) const;
    bool loadGame(const std::string& filename, int& turn, int& maxTurn, std::vector<Player*>& players, std::vector<int>& turnOrder, int& currentPlayerIndex, Board& board, TransactionLogger& logger) const;

    // Cek apakah file sudah ada
    static bool fileExists(const std::string& filename);
};