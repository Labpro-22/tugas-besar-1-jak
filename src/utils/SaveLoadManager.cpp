#include "utils/SaveLoadManager.hpp"
#include "models/Player.hpp"
#include "models/Board.hpp"
#include "models/Tile.hpp"
#include "models/PropertyTile.hpp"
#include "models/SkillCard.hpp"
#include "models/CardDeck.hpp"
#include "utils/TransactionLogger.hpp"
#include "utils/NimonspoliException.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <sys/stat.h>
#include <filesystem>

// ===== fileExists =====
bool SaveLoadManager::fileExists(const std::string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

// ===== saveGame =====
bool SaveLoadManager::saveGame(const std::string& filename, int turn, int maxTurn, const std::vector<Player*>& players, const std::vector<int>& turnOrder, int currentPlayerIndex, const Board& board, const CardDeck<SkillCard>& skillDeck, const TransactionLogger& logger) const {
    // Bikin folder khusus saves
    system("mkdir -p saves");
    std::ofstream out(filename);
    if (!out.is_open()) {
        throw FileNotFoundException("Gagal membuat file: " + filename);
    }

    // Baris 1: <TURN_SAAT_INI> <MAX_TURN>
    out << turn << " " << maxTurn << "\n";

    // Baris 2: <JUMLAH_PEMAIN>
    out << players.size() << "\n";

    // State tiap pemain
    for (const Player* player : players) {
        savePlayerState(out, player);
    }

    // Urutan giliran: <USERNAME_1> <USERNAME_2> ... <USERNAME_N>
    for (int i = 0; i < (int)turnOrder.size(); i++) {
        if (i > 0) out << " ";
        out << players[turnOrder[i]]->getUsername();
    }
    out << "\n";

    // Giliran aktif saat ini
    out << players[currentPlayerIndex]->getUsername() << "\n";

    // State properti
    savePropertyState(out, board);

    // State deck skill
    saveDeckState(out, skillDeck);

    // State log
    saveLogState(out, logger);

    out.close();
    return true;
}

// ===== savePlayerState =====
void SaveLoadManager::savePlayerState(std::ofstream& out, const Player* player) const {
    // <USERNAME> <UANG> <POSISI_PETAK> <STATUS>
    out << player->getUsername() << " " << player->getCash() << " "  << player->getPosition() << " " << player->getStatus() << "\n";

    // <JUMLAH_KARTU_TANGAN>
    auto cards = player->getSkillCards();
    out << cards.size() << "\n";

    // Tiap kartu: <JENIS_KARTU> <NILAI_KARTU> <SISA_DURASI>
    // Untuk kartu tanpa nilai atau durasi, kolom dikosongkan
    // Format: "MoveCard 5" / "ShieldCard" / "DiscountCard 30 1"
    for (SkillCard* card : cards) {
        std::string name = card->getName();
        out << name;
        if (name == "MoveCard") {
            out << " " << dynamic_cast<MoveCard*>(card)->getValue();
        } else if (name == "DiscountCard") {
            out << " " << dynamic_cast<DiscountCard*>(card)->getValue()
                << " " << dynamic_cast<DiscountCard*>(card)->getDuration();
        } else if (name == "ShieldCard") {
            out << " " << dynamic_cast<ShieldCard*>(card)->getDuration();
        }
        out << "\n";
    }
}

// ===== savePropertyState =====
void SaveLoadManager::savePropertyState(std::ofstream& out, const Board& board) const {
    // Kumpulkan semua PropertyTile dulu
    std::vector<PropertyTile*> props;
    for (int i = 0; i < board.getTileCount(); i++) {
        Tile* tile = board.getTile(i);
        PropertyTile* prop = dynamic_cast<PropertyTile*>(tile);
        if (prop) props.push_back(prop);
    }

    // <JUMLAH_PROPERTI>
    out << props.size() << "\n";

    // <KODE_PETAK> <JENIS> <PEMILIK> <STATUS> <FMULT> <FDUR> <N_BANGUNAN>
    for (PropertyTile* prop : props) {
        std::string kode = prop->getCode();
        std::string pemilik = prop->getOwner() ? prop->getOwner()->getUsername() : "BANK";

        // Tentukan jenis
        std::string jenis = "street";
        if (dynamic_cast<RailroadTile*>(prop)) jenis = "railroad";
        else if (dynamic_cast<UtilityTile*>(prop))  jenis = "utility";

        // Status
        std::string statusStr = "BANK";
        PropertyStatus ps = prop->getStatus();
        if (ps == PropertyStatus::OWNED) statusStr = "OWNED";
        else if (ps == PropertyStatus::MORTGAGED) statusStr = "MORTGAGED";

        // Festival multiplier dan durasi (hanya untuk StreetTile)
        int fmult = 1, fdur = 0;
        std::string nBangunan = "0";

        StreetTile* st = dynamic_cast<StreetTile*>(prop);
        if (st) {
            fmult = st->getFestivalMultiplier();
            fdur = st->getFestivalDuration();
            int level = st->getBuildingLevel();
            if (level == 5) nBangunan = "H";
            else nBangunan = std::to_string(level);
        }

        out << kode << " " << jenis << " " << pemilik << " " << statusStr << " " << fmult << " " << fdur << " " << nBangunan << "\n";
    }
}

// ===== saveDeckState =====
void SaveLoadManager::saveDeckState(std::ofstream& out, const CardDeck<SkillCard>& skillDeck) const {
    auto names = skillDeck.getCardNames();
    out << names.size() << "\n";
    for (const auto& name : names) {
        out << name << "\n";
    }
}

// ===== saveLogState =====
void SaveLoadManager::saveLogState(std::ofstream& out, const TransactionLogger& logger) const {
    out << logger.exportState();
}

// ===== loadGame =====
bool SaveLoadManager::loadGame(const std::string& filename, int& turn, int& maxTurn, std::vector<Player*>& players, std::vector<int>& turnOrder, 
        int& currentPlayerIndex, Board& board, CardDeck<SkillCard>& skillDeck, TransactionLogger& logger) const {
    if (!fileExists(filename)) {
        throw FileNotFoundException("File tidak ditemukan: " + filename);
    }

    std::ifstream in(filename);
    if (!in.is_open()) {
        throw FileNotFoundException("Gagal membuka file: " + filename);
    }

    try {
        // Baris 1: <TURN_SAAT_INI> <MAX_TURN>
        if (!(in >> turn >> maxTurn)) {
            throw InvalidInputException("Format file rusak: baris turn.");
        }

        // Baris 2: <JUMLAH_PEMAIN>
        int jumlahPemain;
        if (!(in >> jumlahPemain) || jumlahPemain < 2 || jumlahPemain > 4) {
            throw InvalidInputException("Format file rusak: jumlah pemain tidak valid.");
        }
        in.ignore(); // buang newline

        // Bersihkan players lama
        for (Player* p : players) delete p;
        players.clear();

        // Load tiap pemain
        for (int i = 0; i < jumlahPemain; i++) {
            Player* p = loadPlayerState(in);
            if (!p) throw InvalidInputException("Format file rusak: state pemain.");
            players.push_back(p);
        }

        // Urutan giliran: <USERNAME_1> ... <USERNAME_N>
        std::string turnLine;
        std::getline(in, turnLine);
        std::istringstream turnSS(turnLine);
        std::string uname;
        turnOrder.clear();
        while (turnSS >> uname) {
            // Cari indeks pemain dengan username ini
            for (int i = 0; i < (int)players.size(); i++) {
                if (players[i]->getUsername() == uname) {
                    turnOrder.push_back(i);
                    break;
                }
            }
        }

        // Giliran aktif
        std::string activeUsername;
        std::getline(in, activeUsername);
        // trim whitespace
        activeUsername.erase(0, activeUsername.find_first_not_of(" \t\r\n"));
        activeUsername.erase(activeUsername.find_last_not_of(" \t\r\n") + 1);
        currentPlayerIndex = 0;
        for (int i = 0; i < (int)players.size(); i++) {
            if (players[i]->getUsername() == activeUsername) {
                currentPlayerIndex = i;
                break;
            }
        }

        // Load state properti
        loadPropertyState(in, board, players);

        // Load state deck
        std::vector<std::string> deckTypes = loadDeckState(in);
        
        // rebuild deck
        skillDeck.clear();
        for (const auto& type : deckTypes) {
            SkillCard* card = createSkillCard(type, 0, 0);
            if (card) skillDeck.addCardToDeck(card);
        }
        // Load state log
        loadLogState(in, logger);

    } catch (const NimonspoliException&) {
        in.close();
        throw; // lempar ulang ke Game untuk di-handle
    }

    in.close();
    return true;
}

// ===== loadPlayerState =====
Player* SaveLoadManager::loadPlayerState(std::ifstream& in) const {
    std::string line;
    if (!std::getline(in, line)) return nullptr;

    std::istringstream ss(line);
    std::string username, status;
    int cash, position;

    if (!(ss >> username >> cash >> position >> status)) return nullptr;

    Player* player = new Player(username);
    player->setCash(cash);
    player->setPosition(position);
    player->setStatus(status);

    // Baca kartu tangan
    int jumlahKartu;
    if (!(in >> jumlahKartu)) return player;
    in.ignore();

    for (int i = 0; i < jumlahKartu; i++) {
        std::string cardLine;
        if (!std::getline(in, cardLine)) break;

        std::istringstream cardSS(cardLine);
        std::string cardType;
        int value = 0, duration = 0;
        cardSS >> cardType;
        cardSS >> value;
        cardSS >> duration;

        SkillCard* card = createSkillCard(cardType, value, duration);
        if (card) player->addCard(card);
    }

    return player;
}

// ===== loadPropertyState =====
void SaveLoadManager::loadPropertyState(std::ifstream& in, Board& board, const std::vector<Player*>& players) const {
    int jumlahProperti;
    if (!(in >> jumlahProperti)) return;
    in.ignore();

    for (int i = 0; i < jumlahProperti; i++) {
        std::string line;
        if (!std::getline(in, line)) break;

        std::istringstream ss(line);
        std::string kode, jenis, pemilikName, statusStr;
        int fmult, fdur;
        std::string nBangunan;

        if (!(ss >> kode >> jenis >> pemilikName >> statusStr >> fmult >> fdur >> nBangunan)) continue;

        // Cari tile di board berdasarkan kode
        PropertyTile* prop = nullptr;
        for (int j = 0; j < board.getTileCount(); j++) {
            Tile* tile = board.getTile(j);
            if (tile && tile->getCode() == kode) {
                prop = dynamic_cast<PropertyTile*>(tile);
                break;
            }
        }
        if (!prop) continue;

        // Restore kepemilikan
        if (pemilikName != "BANK") {
            for (Player* p : players) {
                if (p->getUsername() == pemilikName) {
                    prop->changeOwner(p);
                    p->addProperty(prop);
                    break;
                }
            }
        }

        // Restore status
        if (statusStr == "OWNED") prop->redeem();   // set ke OWNED
        else if (statusStr == "MORTGAGED") prop->mortgage(); // set ke MORTGAGED

        // Restore festival dan bangunan untuk StreetTile
        StreetTile* st = dynamic_cast<StreetTile*>(prop);
        if (st) {
            // Restore festival
            // fmult: 1=tidak aktif, 2=1x, 4=2x, 8=3x
            st->setFestivalMultiplier(fmult);

            // Restore durasi festival
            // applyFestival() set durasi = 3, kurangi sesuai fdur
            // fdur di file = durasi tersisa
            st->setFestivalDuration(fdur);

            // Restore bangunan
            if (nBangunan == "H") {
                st->setBuildingLevel(5);
            } else {
                st->setBuildingLevel(std::stoi(nBangunan));
            }
        }
    }
}

// ===== loadDeckState =====
std::vector<std::string> SaveLoadManager::loadDeckState(std::ifstream& in) const {
    std::vector<std::string> cardTypes;
    int jumlahKartu;
    if (!(in >> jumlahKartu)) return cardTypes;
    in.ignore();

    for (int i = 0; i < jumlahKartu; i++) {
        std::string line;
        if (!std::getline(in, line)) break;
        cardTypes.push_back(line);
    }
    return cardTypes;
}

// ===== loadLogState =====
void SaveLoadManager::loadLogState(std::ifstream& in, TransactionLogger& logger) const {
    int jumlahLog;
    if (!(in >> jumlahLog)) return;
    in.ignore();

    for (int i = 0; i < jumlahLog; i++) {
        std::string line;
        if (!std::getline(in, line)) break;
        logger.addLog(line);
    }
}

// ===== createSkillCard =====
SkillCard* SaveLoadManager::createSkillCard(const std::string& type, int value, int duration) const {
    if (type == "MoveCard") return new MoveCard(value > 0 ? value : 3);
    if (type == "DiscountCard") return new DiscountCard(value > 0 ? value : 10, duration > 0 ? duration : 1);
    if (type == "ShieldCard") return new ShieldCard(duration > 0 ? duration : 1);
    if (type == "TeleportCard") return new TeleportCard();
    if (type == "LassoCard") return new LassoCard();
    if (type == "DemolitionCard") return new DemolitionCard();
    return nullptr;
}