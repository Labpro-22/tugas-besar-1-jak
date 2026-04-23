#include "views/CLIRenderer.hpp"
#include "models/Board.hpp"
#include "models/Tile.hpp"
#include "models/PropertyTile.hpp"
#include "models/Player.hpp"
#include "models/SkillCard.hpp"
#include "utils/NimonspoliException.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <map>

// ===== ANSI Color Constants =====
const std::string CLIRenderer::RESET = "\033[0m";
const std::string CLIRenderer::BOLD = "\033[1m";
const std::string CLIRenderer::COLOR_COKLAT = "\033[33m";
const std::string CLIRenderer::COLOR_BIRU_MUDA = "\033[96m";
const std::string CLIRenderer::COLOR_MERAH_MUDA = "\033[95m";
const std::string CLIRenderer::COLOR_ORANGE = "\033[91m";
const std::string CLIRenderer::COLOR_MERAH = "\033[31m";
const std::string CLIRenderer::COLOR_KUNING = "\033[93m";
const std::string CLIRenderer::COLOR_HIJAU = "\033[32m";
const std::string CLIRenderer::COLOR_BIRU_TUA = "\033[34m";
const std::string CLIRenderer::COLOR_DEFAULT = "\033[37m";

// ===== getColorCode =====
std::string CLIRenderer::getColorCode(const std::string& colorGroup) {
    if (colorGroup == "COKLAT") return COLOR_COKLAT;
    if (colorGroup == "BIRU_MUDA") return COLOR_BIRU_MUDA;
    if (colorGroup == "MERAH_MUDA") return COLOR_MERAH_MUDA;
    if (colorGroup == "ORANGE") return COLOR_ORANGE;
    if (colorGroup == "MERAH") return COLOR_MERAH;
    if (colorGroup == "KUNING") return COLOR_KUNING;
    if (colorGroup == "HIJAU") return COLOR_HIJAU;
    if (colorGroup == "BIRU_TUA") return COLOR_BIRU_TUA;
    return COLOR_DEFAULT;
}

// ===== getColorTag =====
std::string CLIRenderer::getColorTag(const std::string& colorGroup) {
    if (colorGroup == "COKLAT") return "[CK]";
    if (colorGroup == "BIRU_MUDA") return "[BM]";
    if (colorGroup == "MERAH_MUDA") return "[PK]";
    if (colorGroup == "ORANGE") return "[OR]";
    if (colorGroup == "MERAH") return "[MR]";
    if (colorGroup == "KUNING") return "[KN]";
    if (colorGroup == "HIJAU") return "[HJ]";
    if (colorGroup == "BIRU_TUA") return "[BT]";
    if (colorGroup == "ABU_ABU") return "[AB]";
    return "[DF]";
}

// ===== visibleLength =====
// Hitung panjang string (ga termasuk ANSI escape codes)
int CLIRenderer::visibleLength(const std::string& s) {
    int len = 0;
    bool inEscape = false;
    for (char c : s) {
        if (c == '\033') {
            inEscape = true;
        } else if (inEscape && c == 'm') {
            inEscape = false;
        } else if (!inEscape) {
            len++;
        }
    }
    return len;
}

// ===== padTo =====
// Pad string ke lebar tertentu, (termasuk ANSI codes)
std::string CLIRenderer::padTo(const std::string& s, int width) {
    int visible = visibleLength(s);
    if (visible >= width) {
        return s;
    }
    return s + std::string(width - visible, ' ');
}

// ===== makeSeparatorRow =====
std::string CLIRenderer::makeSeparatorRow(int numCells) const {
    std::string sep = "+";
    for (int i = 0; i < numCells; i++) {
        sep = sep + std::string(CELL_WIDTH, '-') + "+";
    }
    return sep;
}

// ===== getPlayersAtTile =====
std::vector<int> CLIRenderer::getPlayersAtTile(int tileIndex, const std::vector<Player*>& players) const {
    std::vector<int> result;
    for (int i = 0; i < (int)players.size(); i++) {
        if (players[i] && players[i]->getPosition() == tileIndex && players[i]->getStatus() != "BANKRUPT") {
            result.push_back(i);
        }
    }
    return result;
}

// ===== formatPlayerSymbols =====
// "P1 ^" untuk normal, "P1(1)" untuk ditahan, "P1(V)" untuk mampir
std::string CLIRenderer::formatPlayerSymbols(const std::vector<int>& playerIndices, const std::vector<Player*>& players) const {
    std::string result;
    for (int idx : playerIndices) {
        if (!result.empty()) {
            result += " ";
        }
        result += "P" + std::to_string(idx + 1);
        if (players[idx]->getStatus() == "JAILED") {
            result += "(1)";
        } else {
            result += " ^";
        }
    }
    return result;
}

// ===== getCenterLine =====
// Kembalikan konten baris ke-N di panel tengah papan
std::string CLIRenderer::getCenterLine(int row, int currentTurn, int maxTurn, int currentPlayerIndex, const std::vector<Player*>& players) const {
    switch (row) {
        case 0: return "================================";
        case 1: return "||        NIMONSPOLI         ||";
        case 2: return "================================";
        case 3: return "TURN " + std::to_string(currentTurn) + " / " + std::to_string(maxTurn);
        case 4: return "Giliran: " + (players[currentPlayerIndex]
                        ? players[currentPlayerIndex]->getUsername()
                        : "???");
        case 5: return "--------------------------------";
        case 6: return "LEGENDA KEPEMILIKAN & STATUS";
        case 7: return "P1-P4 : Properti milik Pemain";
        case 8: return "^=L1 ^^=L2 ^^^=L3 *=Hotel";
        case 9: return "(1)=Tahanan  V=Mampir";
        case 10: return "KODE WARNA:";
        case 11: return COLOR_COKLAT + "[CK]=Coklat" + RESET + "  " +
                        COLOR_MERAH + "[MR]=Merah" + RESET;
        case 12: return COLOR_BIRU_MUDA + "[BM]=Biru Muda" + RESET + " " +
                        COLOR_KUNING + "[KN]=Kuning" + RESET;
        case 13: return COLOR_MERAH_MUDA + "[PK]=Pink" + RESET + "     " +
                        COLOR_HIJAU + "[HJ]=Hijau" + RESET;
        case 14: return COLOR_ORANGE + "[OR]=Orange" + RESET + "   " +
                        COLOR_BIRU_TUA + "[BT]=Biru Tua" + RESET;
        case 15: return "[DF]=Aksi      [AB]=Utilitas";
        default: return "";
    }
}

// ===== renderTopRow =====
// Petak indeks 0-10 (GO di kanan, PPJ di kiri — baris atas)
void CLIRenderer::renderTopRow(const Board& board, const std::vector<Player*>& players) const {
    std::cout << makeSeparatorRow(SIDE_SIZE) << "\n";

    // Baris nama petak
    std::cout << "|";
    for (int i = 0; i < SIDE_SIZE; i++) {
        Tile* tile = board.getTile(i);
        if (!tile) { std::cout << padTo("", CELL_WIDTH) << "|"; continue; }
        std::string color = getColorCode(tile->getDisplayColor());
        std::string tag   = getColorTag(tile->getDisplayColor());
        std::string label = color + tag + " " + tile->getCode() + RESET;
        std::cout << padTo(label, CELL_WIDTH) << "|";
    }
    std::cout << "\n";

    // Baris pemain
    std::cout << "|";
    for (int i = 0; i < SIDE_SIZE; i++) {
        auto here = getPlayersAtTile(i, players);
        std::cout << padTo(formatPlayerSymbols(here, players), CELL_WIDTH) << "|";
    }
    std::cout << "\n";

    std::cout << makeSeparatorRow(SIDE_SIZE) << "\n";
}

// ===== renderBottomRow =====
// Petak indeks 20-30 (kanan ke kiri)
void CLIRenderer::renderBottomRow(const Board& board, const std::vector<Player*>& players) const {
    std::cout << makeSeparatorRow(SIDE_SIZE) << "\n";

    std::cout << "|";
    for (int i = 20; i <= 30; i++) {
        Tile* tile = board.getTile(i);
        if (!tile) { 
            std::cout << padTo("", CELL_WIDTH) << "|"; continue; 
        }
        std::string color = getColorCode(tile->getDisplayColor());
        std::string tag   = getColorTag(tile->getDisplayColor());
        std::string label = color + tag + " " + tile->getCode() + RESET;
        std::cout << padTo(label, CELL_WIDTH) << "|";
    }
    std::cout << "\n";

    std::cout << "|";
    for (int i = 20; i <= 30; i++) {
        auto here = getPlayersAtTile(i, players);
        std::cout << padTo(formatPlayerSymbols(here, players), CELL_WIDTH) << "|";
    }
    std::cout << "\n";

    std::cout << makeSeparatorRow(SIDE_SIZE) << "\n";
}

// ===== renderMiddleRows =====
// Sisi kiri (19 turun ke 11) + panel tengah + sisi kanan (31 naik ke 39)
void CLIRenderer::renderMiddleRows(const Board& board, const std::vector<Player*>& players, int currentTurn, int maxTurn, int currentPlayerIndex) const {
    // Indeks sisi kiri dan kanan
    int leftIndices[]  = {19, 18, 17, 16, 15, 14, 13, 12, 11};
    int rightIndices[] = {31, 32, 33, 34, 35, 36, 37, 38, 39};

    // Lebar panel tengah = 9 sel × (CELL_WIDTH + 1) - 1
    int centerWidth = 9 * (CELL_WIDTH + 1) - 1;

    for (int row = 0; row < 9; row++) {
        int leftIdx  = leftIndices[row];
        int rightIdx = rightIndices[row];

        Tile* leftTile  = board.getTile(leftIdx);
        Tile* rightTile = board.getTile(rightIdx);

        // === Baris 1: nama petak ===
        std::cout << "|";

        // Kiri
        if (leftTile) {
            std::string color = getColorCode(leftTile->getDisplayColor());
            std::string label = color + getColorTag(leftTile->getDisplayColor()) + " " + leftTile->getCode() + RESET;
            std::cout << padTo(label, CELL_WIDTH);
        } else {
            std::cout << padTo("", CELL_WIDTH);
        }
        std::cout << "|";

        // Tengah — baris konten panel
        std::string centerContent = getCenterLine(row, currentTurn, maxTurn, currentPlayerIndex, players);
        int padLeft = (centerWidth - visibleLength(centerContent)) / 2;
        if (padLeft < 0) padLeft = 0;
        std::string centeredLine = std::string(padLeft, ' ') + centerContent;
        std::cout << padTo(centeredLine, centerWidth);
        std::cout << "|";

        // Kanan
        if (rightTile) {
            std::string color = getColorCode(rightTile->getDisplayColor());
            std::string label = color + getColorTag(rightTile->getDisplayColor()) + " " + rightTile->getCode() + RESET;
            std::cout << padTo(label, CELL_WIDTH);
        } else {
            std::cout << padTo("", CELL_WIDTH);
        }
        std::cout << "|\n";

        // === Baris 2: pemain ===
        std::cout << "|";

        auto leftPlayers = getPlayersAtTile(leftIdx, players);
        std::cout << padTo(formatPlayerSymbols(leftPlayers, players), CELL_WIDTH) << "|";

        // Tengah — baris konten panel lanjutan (row + 9)
        std::string centerContent2 = getCenterLine(row + 9, currentTurn, maxTurn, currentPlayerIndex, players);
        int padLeft2 = (centerWidth - visibleLength(centerContent2)) / 2;
        if (padLeft2 < 0) padLeft2 = 0;
        std::string centeredLine2 = std::string(padLeft2, ' ') + centerContent2;
        std::cout << padTo(centeredLine2, centerWidth);
        std::cout << "|";

        auto rightPlayers = getPlayersAtTile(rightIdx, players);
        std::cout << padTo(formatPlayerSymbols(rightPlayers, players), CELL_WIDTH) << "|\n";

        // Separator per baris sisi (kiri dan kanan saja)
        std::cout << "+" << std::string(CELL_WIDTH, '-') << "+"
                  << std::string(centerWidth, ' ')
                  << "+" << std::string(CELL_WIDTH, '-') << "+\n";
    }
}

// ===== renderBoard =====
void CLIRenderer::renderBoard(const Board& board, const std::vector<Player*>& players, int currentTurn, int maxTurn, int currentPlayerIndex) const {
    std::cout << "\n";
    renderTopRow(board, players);
    renderMiddleRows(board, players, currentTurn, maxTurn, currentPlayerIndex);
    renderBottomRow(board, players);
    std::cout << "\n";
}

// ===== printAkta =====
void CLIRenderer::printAkta(const StreetTile& tile) const {
    std::cout << "\n";
    std::cout << "+==================================+\n";
    std::cout << "|         AKTA KEPEMILIKAN         |\n";

    std::string colorGroup = tile.getColorGroup();
    std::string colorCode  = getColorCode(colorGroup);
    std::cout << "| " << colorCode << "[" << colorGroup << "] " << tile.getName() << " (" << tile.getCode() << ")" << RESET << " |\n";

    std::cout << "| Kode: " << std::left << std::setw(27) << tile.getCode() << "|\n";
    std::cout << "+==================================+\n";
    std::cout << "| Harga Beli       : M" << std::left << std::setw(13) << tile.getPrice() << "|\n";
    std::cout << "| Nilai Gadai      : M" << std::left << std::setw(13) << tile.getMortgageValue() << "|\n";
    std::cout << "+----------------------------------+\n";

    std::vector<std::string> levels = {
        "unimproved", "1 rumah", "2 rumah", "3 rumah", "4 rumah", "hotel"
    };
    auto rents = tile.getRents();
    for (int i = 0; i < (int)rents.size() && i < 6; i++) {
        std::cout << "| Sewa (" << std::left << std::setw(10) << (levels[i] + ")") << ": M" << std::left << std::setw(10) << rents[i] << "|\n";
    }

    std::cout << "| Harga Rumah : M" << tile.getHouseCost() << "\n";
    std::cout << "| Harga Hotel : M" << tile.getHotelCost() << "\n";

    std::cout << "+==================================+\n";

    std::string statusStr = "";
    PropertyStatus ps = tile.getStatus();
    if (ps == PropertyStatus::BANK) statusStr = "BANK";
    else if (ps == PropertyStatus::OWNED) statusStr = "OWNED";
    else if (ps == PropertyStatus::MORTGAGED) statusStr = "MORTGAGED";
    if (tile.getOwner()) statusStr += " (" + tile.getOwner()->getUsername() + ")";

    std::cout << "| Status : " << std::left << std::setw(24) << statusStr << "|\n";
    std::cout << "+==================================+\n\n";
}

// ===== printAktaNotFound =====
void CLIRenderer::printAktaNotFound(const std::string& tileCode) const {
    std::cout << "Petak \"" << tileCode << "\" tidak ditemukan atau bukan properti.\n";
}

// ===== printPropertyInventory =====
void CLIRenderer::printPropertyInventory(const Player& player) const {
    auto properties = player.getOwnedProperties();

    std::cout << "\n=== Properti Milik: " << player.getUsername() << " ===\n\n";

    if (properties.empty()) {
        std::cout << "Kamu belum memiliki properti apapun.\n\n";
        return;
    }

    // Kelompokkan per warna
    std::map<std::string, std::vector<PropertyTile*>> byColor;
    for (auto* prop : properties) {
        StreetTile* st = dynamic_cast<StreetTile*>(prop);
        std::string color = st ? st->getColorGroup() : "DEFAULT";
        byColor[color].push_back(prop);
    }

    // Urutan warna sesuai papan (clockwise dari GO)
    std::vector<std::string> colorOrder = {
        "COKLAT", "BIRU_MUDA", "MERAH_MUDA", "ORANGE",
        "MERAH", "KUNING", "HIJAU", "BIRU_TUA", "DEFAULT"
    };

    int totalWealth = 0;
    for (const auto& color : colorOrder) {
        if (byColor.find(color) == byColor.end()) continue;

        std::string colorCode = getColorCode(color);
        if (color == "DEFAULT") {
            std::cout << "[STASIUN/UTILITAS]\n";
        } else {
            std::cout << colorCode << "[" << color << "]" << RESET << "\n";
        }

        for (auto* prop : byColor[color]) {
            std::string status = prop->isMortgaged() ? "MORTGAGED [M]" : "OWNED";

            std::string buildInfo = "";
            StreetTile* st = dynamic_cast<StreetTile*>(prop);
            if (st) {
                int level = st->getBuildingLevel();
                if (level == 5)      buildInfo = "Hotel";
                else if (level > 0)  buildInfo = std::to_string(level) + " rumah";
            }

            std::cout << "  - " << prop->getCode();
            if (!buildInfo.empty()) std::cout << "\t" << buildInfo;
            std::cout << "\tM" << prop->getPrice() << "\t" << status << "\n";

            totalWealth += prop->getPrice();
        }
        std::cout << "\n";
    }

    std::cout << "\nTotal kekayaan properti: M" << totalWealth << "\n\n";
}

// ===== printPlayerStatus =====
void CLIRenderer::printPlayerStatus(const Player& player, const Board& board) const {
    Tile* currentTile = board.getTile(player.getPosition());
    std::string tileName = currentTile ? currentTile->getCode() : "???";

    std::cout << "\n--- Status Pemain ---\n";
    std::cout << "Nama   : " << player.getUsername() << "\n";
    std::cout << "Saldo  : M" << player.getCash() << "\n";
    std::cout << "Posisi : " << tileName << " (indeks " << player.getPosition() << ")\n";
    std::cout << "Status : " << player.getStatus() << "\n";
    std::cout << "Kekayaan Total: M" << player.getTotalWealth() << "\n";

    if (player.getStatus() == "JAILED") {
        std::cout << "Sisa giliran penjara: " << player.getJailTurns() << "\n";
    }
    if (player.isShieldActive()) {
        std::cout << "Shield: AKTIF\n";
    }
    if (player.getDiscountPercent() > 0) {
        std::cout << "Diskon sewa: " << player.getDiscountPercent() << "%\n";
    }

    // Kartu skill
    auto& cards = player.getOwnedSkillCards();
    if (!cards.empty()) {
        std::cout << "Kartu Kemampuan (" << cards.size() << "):\n";
        for (int i = 0; i < (int)cards.size(); i++) {
            std::cout << "  [" << i << "] " << cards[i]->getName() << " - " << cards[i]->getDescription() << "\n";
        }
    }
    std::cout << "--------------------\n\n";
}

// ===== printMainMenu =====
void CLIRenderer::printMainMenu() const {
    std::cout << "\n";
    std::cout << "================================\n";
    std::cout << "||        NIMONSPOLI         ||\n";
    std::cout << "================================\n";
    std::cout << "1. New Game\n";
    std::cout << "2. Load Game\n";
    std::cout << "Pilihan: ";
}

// ===== printTurnHeader =====
void CLIRenderer::printTurnHeader(const Player& player, int turn, int maxTurn) const {
    std::cout << "\n--- Turn " << turn << "/" << maxTurn << " | Giliran: " << player.getUsername() << " | Saldo: M" << player.getCash() << " ---\n";
}

// ===== printPrompt =====
void CLIRenderer::printPrompt() const {
    std::cout << "> ";
}

// ===== printError =====
void CLIRenderer::printError(const std::string& message) const {
    std::cerr << "[Error] " << message << "\n";
}

// ===== printInfo =====
void CLIRenderer::printInfo(const std::string& message) const {
    std::cout << message << "\n";
}

// ===== printSuccess =====
void CLIRenderer::printSuccess(const std::string& message) const {
    std::cout << "[OK] " << message << "\n";
}

// ===== printWinner =====
void CLIRenderer::printWinner(const Player& winner, int turn) const {
    std::cout << "\n";
    std::cout << "================================\n";
    std::cout << "         PERMAINAN SELESAI!     \n";
    std::cout << "================================\n";
    std::cout << "PEMENANG: " << winner.getUsername() << "\n";
    std::cout << "Kekayaan Total: M" << winner.getTotalWealth() << "\n";
    std::cout << "Diselesaikan pada Turn: " << turn << "\n";
    std::cout << "================================\n\n";
}