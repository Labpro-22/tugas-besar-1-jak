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

// ===== buildRow2String =====
// Menggabungkan Info Kepemilikan (Kiri) dan Posisi Pemain (Kanan)
static std::string buildRow2String(Tile* tile, int tileIndex, const std::vector<Player*>& players) {
    std::string leftStr = "";
    std::string rightStr = "";

    // Cek Kepemilikan dan Level Bangunan
    PropertyTile* prop = dynamic_cast<PropertyTile*>(tile);
    if (prop && prop->getOwner()) {
        int ownerIdx = -1;
        for (int i = 0; i < (int)players.size(); i++) {
            if (players[i] == prop->getOwner()) {
                ownerIdx = i;
                break;
            }
        }
        if (ownerIdx != -1) {
            leftStr = "P" + std::to_string(ownerIdx + 1);
            StreetTile* street = dynamic_cast<StreetTile*>(prop);
            if (street) {
                int lvl = street->getBuildingLevel();
                if (lvl == 1) leftStr += " ^";
                else if (lvl == 2) leftStr += " ^^";
                else if (lvl == 3) leftStr += " ^^^";
                else if (lvl >= 4) leftStr += " *";
            }
        }
    }

    // Cek Posisi Bidak Pemain
    std::vector<int> here;
    for (int i = 0; i < (int)players.size(); i++) {
        if (players[i] && players[i]->getPosition() == tileIndex && players[i]->getStatus() != "BANKRUPT") {
            here.push_back(i);
        }
    }

    if (!here.empty()) {
        if (tileIndex == 10) { // Khusus Penjara (Index 10)
            std::string inStr = "", vStr = "";
            for (int pIdx : here) {
                if (players[pIdx]->getStatus() == "JAILED") {
                    if (!inStr.empty()) inStr += ",";
                    inStr += std::to_string(pIdx + 1);
                } else {
                    if (!vStr.empty()) vStr += ",";
                    vStr += std::to_string(pIdx + 1);
                }
            }
            if (!inStr.empty()) rightStr += "IN:" + inStr + " ";
            if (!vStr.empty()) rightStr += "V:" + vStr;
        } else { // Petak Normal
            rightStr += "(";
            for (size_t i = 0; i < here.size(); i++) {
                rightStr += std::to_string(here[i] + 1);
                if (i < here.size() - 1) rightStr += ",";
            }
            rightStr += ")";
        }
    }

    // Gabungkan (kalau dua-duanya ada, pisahkan pakai spasi)
    if (!leftStr.empty() && !rightStr.empty()) return leftStr + " " + rightStr;
    if (!leftStr.empty()) return leftStr;
    return rightStr;
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
    // Border atas
    for (int i = 20; i <= 30; i++) {
        Tile* t = board.getTile(i);
        std::string c = t ? getColorCode(t->getDisplayColor()) : COLOR_DEFAULT;
        std::cout << c << "+" << std::string(CELL_WIDTH, '-');
    }
    std::cout << RESET << "+\n";

    // Baris nama petak
    for (int i = 20; i <= 30; i++) {
        Tile* t = board.getTile(i);
        std::string c = t ? getColorCode(t->getDisplayColor()) : COLOR_DEFAULT;
        std::string tag = t ? getColorTag(t->getDisplayColor()) : "[DF]";
        std::string code = t ? t->getCode() : "";
        std::string label = c + tag + " " + code + RESET;
        std::cout << c << "|" << RESET << padTo(label, CELL_WIDTH);
    }
    std::cout << RESET << "|\n";

    // Baris pemain
    for (int i = 20; i <= 30; i++) {
        Tile* t = board.getTile(i);
        std::string c = t ? getColorCode(t->getDisplayColor()) : COLOR_DEFAULT;
        std::cout << c << "|" << RESET << padTo(buildRow2String(t, i, players), CELL_WIDTH);
    }
    std::cout << RESET << "|\n";

    // Border Bawah
    for (int i = 20; i <= 30; i++) {
        Tile* t = board.getTile(i);
        std::string c = t ? getColorCode(t->getDisplayColor()) : COLOR_DEFAULT;
        std::cout << c << "+" << std::string(CELL_WIDTH, '-');
    }
    std::cout << RESET << "+\n";
}

// ===== renderMiddleRows =====
// Sisi kiri (19 turun ke 11) + panel tengah + sisi kanan (31 naik ke 39)
void CLIRenderer::renderMiddleRows(const Board& board, const std::vector<Player*>& players, int currentTurn, int maxTurn, int currentPlayerIndex) const {
    // Indeks sisi kiri, kanan, dan lebar tengah
    int leftIndices[]  = {19, 18, 17, 16, 15, 14, 13, 12, 11};
    int rightIndices[] = {31, 32, 33, 34, 35, 36, 37, 38, 39};
    int centerWidth = 9 * (CELL_WIDTH + 1) - 1;

    for (int row = 0; row < 9; row++) {
        int leftIdx  = leftIndices[row];
        int rightIdx = rightIndices[row];

        Tile* leftTile  = board.getTile(leftIdx);
        Tile* rightTile = board.getTile(rightIdx);

        std::string cLeft = leftTile ? getColorCode(leftTile->getDisplayColor()) : COLOR_DEFAULT;
        std::string cRight = rightTile ? getColorCode(rightTile->getDisplayColor()) : COLOR_DEFAULT;

        // === Baris 1: nama petak ===
        std::string leftLabel = cLeft + (leftTile ? getColorTag(leftTile->getDisplayColor()) + " " + leftTile->getCode() : "") + RESET;
        std::cout << cLeft << "|" << RESET << padTo(leftLabel, CELL_WIDTH) << cLeft << "|" << RESET;

        // Tengah 1
        std::string centerContent = getCenterLine(row, currentTurn, maxTurn, currentPlayerIndex, players);
        int padLeft = (centerWidth - visibleLength(centerContent)) / 2;
        if (padLeft < 0) padLeft = 0;
        std::string centeredLine = std::string(padLeft, ' ') + centerContent;
        std::cout << padTo(centeredLine, centerWidth);

        // Kanan 1
        std::string rightLabel = cRight + (rightTile ? getColorTag(rightTile->getDisplayColor()) + " " + rightTile->getCode() : "") + RESET;
        std::cout << cRight << "|" << RESET << padTo(rightLabel, CELL_WIDTH) << cRight << "|\n" << RESET;

        // === Baris 2: pemain  ===
        std::cout << cLeft << "|" << RESET << padTo(buildRow2String(leftTile, leftIdx, players), CELL_WIDTH) << cLeft << "|" << RESET;

        // Tengah 2
        std::string centerContent2 = getCenterLine(row + 9, currentTurn, maxTurn, currentPlayerIndex, players);
        int padLeft2 = (centerWidth - visibleLength(centerContent2)) / 2;
        if (padLeft2 < 0) padLeft2 = 0;
        std::string centeredLine2 = std::string(padLeft2, ' ') + centerContent2;
        std::cout << padTo(centeredLine2, centerWidth);

        // Kanan 2
        std::cout << cRight << "|" << RESET << padTo(buildRow2String(rightTile, rightIdx, players), CELL_WIDTH) << cRight << "|\n" << RESET;
        
        // Separator per baris sisi
        if (row < 8) {
            std::cout << cLeft << "+" << std::string(CELL_WIDTH, '-') << "+" << RESET << std::string(centerWidth, ' ') << cRight << "+" << std::string(CELL_WIDTH, '-') << "+\n" << RESET;
        }
    }
}

// ===== renderBottomRow =====
// Petak indeks 20-30 (kanan ke kiri)
void CLIRenderer::renderBottomRow(const Board& board, const std::vector<Player*>& players) const {
    // Border Atas
    for (int i = 10; i >= 0; i--) {
        Tile* t = board.getTile(i);
        std::string c = t ? getColorCode(t->getDisplayColor()) : COLOR_DEFAULT;
        std::cout << c << "+" << std::string(CELL_WIDTH, '-');
    }
    std::cout << RESET << "+\n";

    // Baris Nama Petak
    for (int i = 10; i >= 0; i--) {
        Tile* t = board.getTile(i);
        std::string c = t ? getColorCode(t->getDisplayColor()) : COLOR_DEFAULT;
        std::string tag = t ? getColorTag(t->getDisplayColor()) : "[DF]";
        std::string code = t ? t->getCode() : "";
        std::string label = c + tag + " " + code + RESET;
        std::cout << c << "|" << RESET << padTo(label, CELL_WIDTH);
    }
    std::cout << RESET << "|\n";

    // Baris Pemain
    for (int i = 10; i >= 0; i--) {
        Tile* t = board.getTile(i);
        std::string c = t ? getColorCode(t->getDisplayColor()) : COLOR_DEFAULT;
        std::cout << c << "|" << RESET << padTo(buildRow2String(t, i, players), CELL_WIDTH);
    }
    std::cout << RESET << "|\n";

    // Border Bawah
    for (int i = 10; i >= 0; i--) {
        Tile* t = board.getTile(i);
        std::string c = t ? getColorCode(t->getDisplayColor()) : COLOR_DEFAULT;
        std::cout << c << "+" << std::string(CELL_WIDTH, '-');
    }
    std::cout << RESET << "+\n";
}

// ===== renderBoard =====
void CLIRenderer::renderBoard(const Board& board, const std::vector<Player*>& players, int currentTurn, int maxTurn, int currentPlayerIndex) const {
    std::cout << "\n";
    renderTopRow(board, players);
    renderMiddleRows(board, players, currentTurn, maxTurn, currentPlayerIndex);
    renderBottomRow(board, players);
    std::cout << "\n";
}

// ===== printDeed =====
// StreetTile
void CLIRenderer::printDeed(const StreetTile& tile) const {
    std::string colorGroup = tile.getColorGroup();
    std::string c = getColorCode(colorGroup);

    std::cout << "\n";
    std::cout << c << "+==================================+" << RESET << "\n";
    std::cout << c << "|" << RESET << BOLD << "         AKTA KEPEMILIKAN         " << RESET << c << "|\n";
    
    std::cout << c << "| " << RESET << c << "[" << colorGroup << "] " << tile.getName() << " (" << tile.getCode() << ")" << RESET << std::string(32 - visibleLength("[" + colorGroup + "] " + tile.getName() + " (" + tile.getCode() + ")"), ' ') << c << " |\n";

    std::cout << c << "| " << RESET << "Kode: " << std::left << std::setw(27) << tile.getCode() << c << "|\n";
    std::cout << c << "+==================================+" << RESET << "\n";
    std::cout << c << "| " << RESET << "Harga Beli       : M" << std::left << std::setw(13) << tile.getPrice() << c << "|\n";
    std::cout << c << "| " << RESET << "Nilai Gadai      : M" << std::left << std::setw(13) << tile.getMortgageValue() << c << "|\n";
    std::cout << c << "+----------------------------------+" << RESET << "\n";

    std::vector<std::string> levels = {
        "unimproved", "1 rumah", "2 rumah", "3 rumah", "4 rumah", "hotel"
    };
    auto rents = tile.getRents();
    for (int i = 0; i < (int)rents.size() && i < 6; i++) {
        std::cout << c << "| " << RESET << "Sewa (" << std::left << std::setw(12) << levels[i] << ") : M" << std::left << std::setw(10) << rents[i] << c << "|\n";
    }

    std::cout << c << "| " << RESET << "Harga Bangun Rumah: M" << std::left << std::setw(11) << tile.getHouseCost() << c << "|\n";
    std::cout << c << "| " << RESET << "Harga Bangun Hotel: M" << std::left << std::setw(11) << tile.getHotelCost() << c << "|\n";

    std::cout << c << "+==================================+" << RESET << "\n";

    std::string statusStr = "";
    PropertyStatus ps = tile.getStatus();
    if (ps == PropertyStatus::BANK) statusStr = "BANK";
    else if (ps == PropertyStatus::OWNED) statusStr = "OWNED";
    else if (ps == PropertyStatus::MORTGAGED) statusStr = "MORTGAGED";
    if (tile.getOwner()) statusStr += " (" + tile.getOwner()->getUsername() + ")";

    std::cout << c << "| " << RESET << "Status : " << COLOR_KUNING << std::left << std::setw(24) << statusStr << RESET << c << "|\n";
    std::cout << c << "+==================================+" << RESET << "\n\n";
}

// RailroadTile
void CLIRenderer::printDeed(const RailroadTile& tile) const {
    std::string c = BOLD;
    std::cout << "\n";
    std::cout << c << "+==================================+" << RESET << "\n";
    std::cout << c << "|         AKTA KEPEMILIKAN         |" << RESET << "\n";
    std::cout << c << "|   [STASIUN] " << std::left << std::setw(21) << (tile.getName() + " (" + tile.getCode() + ")") << "|" << RESET << "\n";
    std::cout << c << "+==================================+" << RESET << "\n";
    std::cout << "| Nilai Gadai : M" << std::left << std::setw(18) << tile.getMortgageValue() << "|\n";
    std::cout << "+----------------------------------+\n";

    for (const auto& entry : tile.getRentTable()) {
        std::string label = "Sewa (" + std::to_string(entry.first) + " stasiun)";
        std::cout << "| " << std::left << std::setw(20) << label << ": M" << std::left << std::setw(12) << entry.second << "|\n";
    }

    std::cout << "+==================================+\n";

    std::string statusStr = "";
    PropertyStatus ps = tile.getStatus();
    if (ps == PropertyStatus::BANK) statusStr = "BANK";
    else if (ps == PropertyStatus::OWNED) statusStr = "OWNED";
    else if (ps == PropertyStatus::MORTGAGED) statusStr = "MORTGAGED";
    if (tile.getOwner()) statusStr += " (" + tile.getOwner()->getUsername() + ")";

    std::string statusColor = (ps == PropertyStatus::OWNED) ? COLOR_HIJAU :  (ps == PropertyStatus::MORTGAGED) ? COLOR_KUNING : "";
    std::cout << "| Status : " << statusColor << std::left << std::setw(24) << statusStr << RESET << "|\n";
    std::cout << "+==================================+\n\n";
}

// UtilityTile
void CLIRenderer::printDeed(const UtilityTile& tile) const {
    std::string c = BOLD;
    std::cout << "\n";
    std::cout << c << "+==================================+" << RESET << "\n";
    std::cout << c << "|         AKTA KEPEMILIKAN         |" << RESET << "\n";
    std::cout << c << "|   [UTILITAS] " << std::left << std::setw(20) << (tile.getName() + " (" + tile.getCode() + ")") << "|" << RESET << "\n";
    std::cout << c << "+==================================+" << RESET << "\n";
    std::cout << "| Nilai Gadai : M" << std::left << std::setw(18) << tile.getMortgageValue() << "|\n";
    std::cout << "+----------------------------------+\n";

    for (const auto& entry : tile.getMultiplierTable()) {
        std::string label = "Pengali (" + std::to_string(entry.first) + " utilitas)";
        std::cout << "| " << std::left << std::setw(20) << label << ": " << std::left << std::setw(14) << (std::to_string(entry.second) + "x dadu") << "|\n";
    }

    std::cout << "+==================================+\n";

    std::string statusStr = "";
    PropertyStatus ps = tile.getStatus();
    if (ps == PropertyStatus::BANK) statusStr = "BANK";
    else if (ps == PropertyStatus::OWNED) statusStr = "OWNED";
    else if (ps == PropertyStatus::MORTGAGED) statusStr = "MORTGAGED";
    if (tile.getOwner()) statusStr += " (" + tile.getOwner()->getUsername() + ")";

    std::string statusColor = (ps == PropertyStatus::OWNED) ? COLOR_HIJAU :  (ps == PropertyStatus::MORTGAGED) ? COLOR_KUNING : "";
    std::cout << "| Status : " << statusColor << std::left << std::setw(24) << statusStr << RESET << "|\n";
    std::cout << "+==================================+\n\n";
}

// ===== printDeeedNotFound =====
void CLIRenderer::printDeedNotFound(const std::string& tileCode) const {
    std::cout << COLOR_KUNING << "Petak \"" << tileCode << "\" tidak ditemukan atau bukan properti." << RESET << "\n";
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

    std::cout << "\n" << COLOR_BIRU_MUDA << BOLD << "📊 --- STATUS PEMAIN ---" << RESET << "\n";
    std::cout << " 👤 " << BOLD << "Nama   : " << RESET << COLOR_MERAH_MUDA << player.getUsername() << RESET << "\n";
    std::cout << " 💰 " << BOLD << "Saldo  : " << RESET << COLOR_KUNING << "M" << player.getCash() << RESET << "\n";
    std::cout << " 📍 " << BOLD << "Posisi : " << RESET << tileName << " (Indeks " << player.getPosition() << ")\n";

    std::cout << " 🎭 " << BOLD << "Status : " << RESET;
    if (player.getStatus() == "JAILED") std::cout << COLOR_MERAH << BOLD << "JAILED 🚔" << RESET << "\n";
    else if (player.getStatus() == "BANKRUPT") std::cout << COLOR_DEFAULT << "BANKRUPT 💀" << RESET << "\n";
    else std::cout << COLOR_HIJAU << "ACTIVE ✅" << RESET << "\n";

    std::cout << " 💎 " << BOLD << "Total Kekayaan: " << RESET << COLOR_KUNING << "M" << player.getTotalWealth() << RESET << "\n";

    if (player.getStatus() == "JAILED") {
        std::cout << "Sisa giliran penjara: " << player.getJailTurns() << "\n";
    }
    if (player.isShieldActive()) {
        std::cout << COLOR_BIRU_MUDA << "Shield: AKTIF" << RESET << "\n";
    }
    if (player.getDiscountPercent() > 0) {
        std::cout << COLOR_HIJAU << "Diskon sewa: " << player.getDiscountPercent() << "%" << RESET << "\n";
    }

    // Kartu skill
    auto& cards = player.getOwnedSkillCards();
    if (!cards.empty()) {
        std::cout << "Kartu Kemampuan (" << cards.size() << "):\n";
        for (int i = 0; i < (int)cards.size(); i++) {
            std::cout << "  [" << i << "] " << cards[i]->getName() << " - " << cards[i]->getDescription() << "\n";
        }
    }
    
    std::cout << COLOR_BIRU_MUDA << BOLD << "------------------------" << RESET << "\n\n";
}

// ===== printMainMenu =====
void CLIRenderer::printMainMenu() const {
std::cout << "\n";
    std::cout << COLOR_BIRU_MUDA << BOLD << "✨================================✨\n";
    std::cout << "||          " << COLOR_MERAH_MUDA << "NIMONSPOLI" << COLOR_BIRU_MUDA << "          ||\n";
    std::cout << "✨================================✨\n" << RESET;
    std::cout << COLOR_KUNING << " 1. 🎲 New Game\n";
    std::cout << " 2. 📂 Load Game\n" << RESET;
    std::cout << COLOR_BIRU_MUDA << BOLD << " Pilihan: " << RESET;
}

// ===== printTurnHeader =====
void CLIRenderer::printTurnHeader(const Player& player, int turn, int maxTurn) const {
    std::cout << "\n" << COLOR_BIRU_TUA << BOLD << "=========================================================" << RESET << "\n";
    std::cout << COLOR_BIRU_MUDA << BOLD << " 🔄 TURN " << turn << " / " << maxTurn << RESET 
              << "  |  " << COLOR_MERAH_MUDA << BOLD << "👤 " << player.getUsername() << RESET 
              << "  |  " << COLOR_KUNING << BOLD << "💰 M" << player.getCash() << RESET << "\n";
    std::cout << COLOR_BIRU_TUA << BOLD << "=========================================================" << RESET << "\n";
}

// ===== printPrompt =====
void CLIRenderer::printPrompt() const {
    std::cout << COLOR_MERAH_MUDA << BOLD << "🌸 > " << RESET;
}

// ===== printHelp =====
void CLIRenderer::printHelp() const { 
    std::cout << "\n" << COLOR_BIRU_MUDA << BOLD << "=================================================================\n";
    std::cout << "                   📜 DAFTAR PERINTAH NIMONSPOLI                 \n";
    std::cout << "=================================================================\n\n" << RESET;

    std::cout << COLOR_KUNING << BOLD << "[ 🎲 AKSI GILIRAN ]\n" << RESET;
    std::cout << "  " << COLOR_BIRU_MUDA << std::left << std::setw(25) << "LEMPAR_DADU" << RESET << ": Melempar dadu untuk bergerak ke petak baru.\n";
    std::cout << "  " << COLOR_BIRU_MUDA << std::left << std::setw(25) << "ATUR_DADU <x> <y>" << RESET << ": [Cheat] Mengatur angka dadu secara manual.\n";
    std::cout << "  " << COLOR_BIRU_MUDA << std::left << std::setw(25) << "AKHIRI_GILIRAN" << RESET << ": Mengakhiri giliranmu & lanjut ke pemain berikutnya.\n\n";

    std::cout << COLOR_KUNING << BOLD << "[ 🏘️ PROPERTI & BANGUNAN ]\n" << RESET;
    std::cout << "  " << COLOR_BIRU_MUDA << std::left << std::setw(25) << "BANGUN <petak>" << RESET << ": Membangun rumah/hotel di properti milikmu.\n\n";

    std::cout << COLOR_KUNING << BOLD << "[ ⚖️ MANAJEMEN ASET & LELANG ]\n" << RESET;
    std::cout << "  " << COLOR_BIRU_MUDA << std::left << std::setw(25) << "GADAI <petak>" << RESET << ": Menggadaikan properti untuk mendapatkan uang.\n";
    std::cout << "  " << COLOR_BIRU_MUDA << std::left << std::setw(25) << "TEBUS <petak>" << RESET << ": Menebus properti yang sedang digadaikan.\n";
    std::cout << "  " << COLOR_BIRU_MUDA << std::left << std::setw(25) << "TAWAR <harga>" << RESET << ": Mengajukan harga saat sesi lelang properti.\n";
    std::cout << "  " << COLOR_BIRU_MUDA << std::left << std::setw(25) << "LEPAS" << RESET << ": Mundur dari sesi lelang saat ini.\n\n";

    std::cout << COLOR_KUNING << BOLD << "[ 🃏 PENJARA & KARTU ]\n" << RESET;
    std::cout << "  " << COLOR_BIRU_MUDA << std::left << std::setw(25) << "GUNAKAN_KARTU_BEBAS" << RESET << ": Menggunakan kartu khusus untuk bebas.\n";
    std::cout << "  " << COLOR_BIRU_MUDA << std::left << std::setw(25) << "GUNAKAN_KEMAMPUAN <i>" << RESET << ": Menggunakan kartu skill (kemampuan) yang dimiliki.\n\n";

    std::cout << COLOR_KUNING << BOLD << "[ 📊 INFORMASI ]\n" << RESET;
    std::cout << "  " << COLOR_BIRU_MUDA << std::left << std::setw(25) << "CETAK_STATUS" << RESET << ": Melihat info saldo, posisi, dan statusmu saat ini.\n";
    std::cout << "  " << COLOR_BIRU_MUDA << std::left << std::setw(25) << "CETAK_PAPAN" << RESET << ": Menampilkan kondisi papan Nimonspoli saat ini.\n";
    std::cout << "  " << COLOR_BIRU_MUDA << std::left << std::setw(25) << "CETAK_PROPERTI" << RESET << ": Menampilkan daftar seluruh properti yang kamu miliki.\n";
    std::cout << "  " << COLOR_BIRU_MUDA << std::left << std::setw(25) << "CETAK_AKTA <petak>" << RESET << ": Melihat detail harga, sewa, & status properti.\n";
    std::cout << "  " << COLOR_BIRU_MUDA << std::left << std::setw(25) << "CETAK_LOG" << RESET << ": Melihat riwayat aksi yang sudah terjadi di dalam game.\n";
    std::cout << "  " << COLOR_BIRU_MUDA << std::left << std::setw(25) << "BANTUAN" << RESET << ": Menampilkan menu daftar perintah ini.\n\n";

    std::cout << COLOR_KUNING << BOLD << "[ 💾 SISTEM ]\n" << RESET;
    std::cout << "  " << COLOR_BIRU_MUDA << std::left << std::setw(25) << "SIMPAN <file>" << RESET << ": Menyimpan progres permainan.\n";
    std::cout << "  " << COLOR_BIRU_MUDA << std::left << std::setw(25) << "QUIT" << RESET << ": Keluar dari aplikasi Nimonspoli sepenuhnya.\n\n";
    
    std::cout << COLOR_BIRU_MUDA << BOLD << "=================================================================\n" << RESET;
}

// ===== printError =====
void CLIRenderer::printError(const std::string& message) const {
    std::cerr << COLOR_MERAH << BOLD << "🚨 [ERROR] " << RESET << COLOR_MERAH << message << RESET << "\n";
}

// ===== printInfo =====
void CLIRenderer::printInfo(const std::string& message) const {
    std::cout << COLOR_BIRU_MUDA << "💡 " << message << RESET << "\n";
}

// ===== printSuccess =====
void CLIRenderer::printSuccess(const std::string& message) const {
    std::cout << COLOR_HIJAU << BOLD << "✨ [SUCCESS] " << RESET << COLOR_HIJAU << message << RESET << "\n";
}

// ===== printWinner =====
void CLIRenderer::printWinner(const Player& winner, int turn) const {
    std::cout << "\n" << BOLD << COLOR_HIJAU;
    std::cout << "================================\n";
    std::cout << "         PERMAINAN SELESAI!     \n";
    std::cout << "================================\n" << RESET;
    std::cout << "PEMENANG: " << winner.getUsername() << RESET << "\n";
    std::cout << "Kekayaan Total: M" << winner.getTotalWealth() << "\n";
    std::cout << "Diselesaikan pada Turn: " << turn << "\n";
    std::cout << "================================\n\n";
}

int CLIRenderer::promptDropCard(const std::vector<std::string>& cardNames) const {
    printInfo("Daftar Kartu Kemampuan Kamu:");
    for (int i = 0; i < (int)cardNames.size(); i++) {
        printInfo(std::to_string(i + 1) + ". " + cardNames[i]);
    }
    
    int dropIndex = -1;
    while (dropIndex < 0 || dropIndex >= (int)cardNames.size()) {
        printInfo("Pilih nomor kartu yang ingin dibuang (1-" + std::to_string(cardNames.size()) + "): ");
        std::string input;
        std::getline(std::cin, input);
        try {
            dropIndex = std::stoi(input) - 1;
        } catch (...) {
            dropIndex = -1;
        }
    }
    return dropIndex;
}

int CLIRenderer::promptChoice() const {
    printInfo("Pilih aksi: ");
    std::string input;
    std::getline(std::cin, input);
    try { return std::stoi(input) - 1; }
    catch (...) { return -1; }
}

std::string CLIRenderer::promptTileCode() const {
    printInfo("Masukkan kode petak tujuan: ");
    std::string kode;
    std::getline(std::cin, kode);
    return kode;
}

int CLIRenderer::promptPropertyChoice(const std::vector<PropertyTile*>& props) const {
    printInfo("=== Properti yang Dapat Digadaikan ===");
    for (size_t i = 0; i < props.size(); ++i) {
        printInfo(std::to_string(i + 1) + ". " + props[i]->getName() + 
            " (" + props[i]->getCode() + ") Nilai Gadai: M" + 
            std::to_string(props[i]->getMortgageValue()));
    }
    printInfo("Pilih nomor properti (0 untuk batal): ");
    std::string input;
    std::getline(std::cin, input);
    try { return std::stoi(input); } catch (...) { return 0; }
}

bool CLIRenderer::promptYesNo(const std::string& question) const {
    printInfo(question + " (y/n): ");
    std::string input;
    while (true) {
        std::getline(std::cin, input);
        if (input == "y" || input == "Y") return true;
        if (input == "n" || input == "N") return false;
        printInfo("Masukkan y atau n: ");
    }
}