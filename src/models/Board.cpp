#include "Board.hpp"
#include "Tile.hpp"
#include "PropertyTile.hpp"
#include "Player.hpp"
#include "NimonspoliException.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <map>

// Constructor
Board::Board() : dynamicBoardEnabled(false)
{
    tiles.clear();
}

// Destructor
Board::~Board()
{
    clearBoard();
}

void Board::initializeBoard(const std::string &configFile)
{
    clearBoard();
    loadTilesFromConfig(configFile);
    validateBoard();
}

void Board::setDynamicBoard(bool enabled, int numTiles)
{
    dynamicBoardEnabled = enabled;
    if (enabled && numTiles >= 20 && numTiles <= 60)
    {
        // Resize board if needed
        while (tiles.size() < static_cast<size_t>(numTiles))
        {
            // Add placeholder tiles (you may need to create appropriate tile types)
            // This is a simplified implementation
            addTile(nullptr); // Placeholder
        }
        while (tiles.size() > static_cast<size_t>(numTiles))
        {
            tiles.pop_back();
        }
    }
}

Tile *Board::getTile(int index) const
{
    if (index < 0 || index >= static_cast<int>(tiles.size()))
    {
        throw NimonspoliException("Invalid tile index: " + std::to_string(index));
    }
    return tiles[index];
}

int Board::getTileCount() const
{
    return tiles.size();
}

void Board::printBoard(const std::vector<Player *> &players) const
{
    if (tiles.empty())
    {
        std::cout << "Board is empty!" << std::endl;
        return;
    }

    // Print head
    std::cout << "\n=== CURRENT BOARD STATE ===" << std::endl;

    // Print top border
    printTopBorder();

    // Print top row [20-29]
    int startIdx = 20;
    int endIdx = std::min(30, getTileCount());
    std::vector<Tile *> topRow;
    for (int i = startIdx; i < endIdx; i++)
    {
        topRow.push_back(tiles[i]);
    }
    printTileRow(topRow, players, startIdx);

    // Sisi kiri [10-19]
    // Sisi kanan [31-40]
    for (int row = 0; row < 9; row++)
    {
        int leftIdx = 19 - row;
        int rightIdx = 30 + row;

        if (leftIdx >= 0 && leftIdx < getTileCount())
        {
            std::string leftName = tiles[leftIdx]->getCode();
            if (leftName.length() > 10)
                leftName = leftName.substr(0, 10);
            std::cout << "| " << leftName;
            for (int i = leftName.length(); i < 12; i++)
                std::cout << " ";
        }
        else
        {
            std::cout << "|            ";
        }

        // Add player positions on left side
        bool hasPlayerLeft = false;
        for (auto *player : players)
        {
            if (player && player->getPosition() == leftIdx)
            {
                std::cout << " " << player->getUsername().substr(0, 2);
                hasPlayerLeft = true;
                break;
            }
        }
        if (!hasPlayerLeft)
            std::cout << "   ";

        std::cout << " |";

        // Center space
        std::cout << "            ";

        // Right side
        if (rightIdx < getTileCount())
        {
            std::string rightName = tiles[rightIdx]->getCode();
            if (rightName.length() > 10)
                rightName = rightName.substr(0, 10);
            std::cout << "| " << rightName;
            for (int i = rightName.length(); i < 12; i++)
                std::cout << " ";

            // Add player positions on right side
            bool hasPlayerRight = false;
            for (auto *player : players)
            {
                if (player && player->getPosition() == rightIdx)
                {
                    std::cout << " " << player->getUsername().substr(0, 2);
                    hasPlayerRight = true;
                    break;
                }
            }
            if (!hasPlayerRight)
                std::cout << "   ";
            std::cout << " |";
        }
        std::cout << std::endl;
    }

    // Print bottom row [0-9]]
    startIdx = 0;
    endIdx = std::min(10, getTileCount());
    std::vector<Tile *> bottomRow;
    for (int i = startIdx; i < endIdx; i++)
    {
        bottomRow.push_back(tiles[i]);
    }
    printTileRow(bottomRow, players, startIdx);

    printBottomBorder();

    // Print ownership legend
    std::cout << "\n"
              << getOwnershipLegend(players) << std::endl;
    std::cout << "============================\n"
              << std::endl;
}

void Board::addTile(Tile *tile)
{
    if (tile == nullptr)
    {
        throw NimonspoliException("[addTile] Tile tidak boleh null");
    }
    tiles.push_back(tile);
}

void Board::removeTile(int index)
{
    if (index < 0 || index >= static_cast<int>(tiles.size()))
    {
        throw NimonspoliException("[removeTile] Index tile invalid");
    }
    delete tiles[index];
    tiles.erase(tiles.begin() + index);
}

void Board::clearBoard()
{
    for (Tile *tile : tiles)
    {
        delete tile;
    }
    tiles.clear();
}

int Board::getStartTileIndex() const
{
    for (size_t i = 0; i < tiles.size(); i++)
    {
        if (tiles[i] && tiles[i]->getCode() == "GO")
        {
            return i;
        }
    }
    return 0; // Default to first tile if GO not found
}

int Board::getJailTileIndex() const
{
    for (size_t i = 0; i < tiles.size(); i++)
    {
        if (tiles[i] && (tiles[i]->getCode() == "JAIL" || tiles[i]->getCode() == "Penjara"))
        {
            return i;
        }
    }
    return 10; // Jail Index
}

bool Board::isValidPosition(int index) const
{
    return index >= 0 && index < static_cast<int>(tiles.size());
}

void Board::loadTilesFromConfig(const std::string &configFile)
{
    std::ifstream file(configFile);
    if (!file.is_open())
    {
        throw NimonspoliException("[loadTilesFromConfig] File Config " + configFile + " gagal dibuka");
    }

    std::string line;
    int lineNum = 0;

    while (std::getline(file, line))
    {
        lineNum++;
        if (line.empty() || line[0] == '#')
            continue; // Skip comments dan line kosong

        std::stringstream ss(line);
        std::string type, code, name;

        std::getline(ss, type, ',');
        std::getline(ss, code, ',');
        std::getline(ss, name, ',');

        // Trim whitespace
        type.erase(0, type.find_first_not_of(" \t"));
        type.erase(type.find_last_not_of(" \t") + 1);
        code.erase(0, code.find_first_not_of(" \t"));
        code.erase(code.find_last_not_of(" \t") + 1);
        name.erase(0, name.find_first_not_of(" \t"));
        name.erase(name.find_last_not_of(" \t") + 1);

        if (type == "PROPERTY" || type == "STREET")
        {
            // Parse street properties
            std::string colorGroup;
            std::vector<int> rents;
            int buyPrice, mortgageValue, houseCost, hotelCost;

            // Parse additional fields
            std::getline(ss, colorGroup, ',');
            // Parse rents (assuming 6 rent values)
            for (int i = 0; i < 6; i++)
            {
                std::string rentStr;
                std::getline(ss, rentStr, ',');
                rents.push_back(std::stoi(rentStr));
            }
            ss >> buyPrice >> mortgageValue >> houseCost >> hotelCost;
        }
        else if (type == "RAILROAD")
        {
            // Parse railroad
            int buyPrice, mortgageValue;
            std::map<int, int> rentTable;
            ss >> buyPrice >> mortgageValue;
            // Parse rent table
            for (int i = 1; i <= 4; i++)
            {
                int rent;
                ss >> rent;
                rentTable[i] = rent;
            }
        }
        else if (type == "UTILITY")
        {
            // Parse utility
            int buyPrice, mortgageValue;
            std::map<int, int> multiplierTable;
            ss >> buyPrice >> mortgageValue;
            // Parse multiplier table (typically for 1-2 utilities)
            for (int i = 1; i <= 2; i++)
            {
                int multiplier;
                ss >> multiplier;
                multiplierTable[i] = multiplier;
            }
            // tile = new UtilityTile(lineNum, code, name, buyPrice, mortgageValue, multiplierTable);
        }
        else if (type == "ACTION")
        {
            // tile = new ActionTile(lineNum, code, name);
        }
    }

    file.close();

    if (tiles.empty())
    {
        throw NimonspoliException("[loadTilesFromConfig] Config file kosong");
    }
}

void Board::validateBoard()
{
    int goCount = 0;
    int jailCount = 0;

    for (Tile *tile : tiles)
    {
        if (tile->getCode() == "GO")
        {
            goCount++;
        }
        if (tile->getCode() == "JAIL" || tile->getCode() == "Penjara")
        {
            jailCount++;
        }
    }

    if (goCount != 1)
    {
        throw NimonspoliException("Board must have exactly 1 GO tile. Found: " + std::to_string(goCount));
    }

    if (jailCount != 1)
    {
        throw NimonspoliException("Board must have exactly 1 JAIL tile. Found: " + std::to_string(jailCount));
    }
}

std::string Board::getTileColorCode(Tile *tile) const
{
    // Color code property tile
    PropertyTile *propTile = dynamic_cast<PropertyTile *>(tile);
    if (propTile)
    {
        StreetTile *streetTile = dynamic_cast<StreetTile *>(propTile);
        if (streetTile)
        {
            // Warna
            return "\033[1;33m"; // Kuning
        }
        return "\033[1;36m"; // Biru
    }
    return "\033[1;37m"; // Putih
}

std::string Board::getOwnershipLegend(const std::vector<Player *> &players) const
{
    std::string legend = "Ownership Legend: ";
    for (auto *player : players)
    {
        if (player && player->getStatus() != "BANKRUPT")
        {
            legend += "[" + player->getUsername().substr(0, 2) + "=" + player->getUsername() + "] ";
        }
    }
    return legend;
}

void Board::printTopBorder() const
{
    std::cout << "+";
    for (int i = 0; i < 78; i++)
        std::cout << "-";
    std::cout << "+" << std::endl;
}

void Board::printBottomBorder() const
{
    std::cout << "+";
    for (int i = 0; i < 78; i++)
        std::cout << "-";
    std::cout << "+" << std::endl;
}

void Board::printTileRow(const std::vector<Tile *> &rowTiles,
                         const std::vector<Player *> &players,
                         int startIndex) const
{
    // Print tile name
    std::cout << "|";
    for (size_t i = 0; i < rowTiles.size(); i++)
    {
        if (rowTiles[i])
        {
            std::string name = rowTiles[i]->getCode();
            if (name.length() > 12)
                name = name.substr(0, 12);
            std::cout << " " << name;
            for (int j = name.length(); j < 13; j++)
                std::cout << " ";
        }
        else
        {
            std::cout << "             ";
        }
        std::cout << "|";
    }
    std::cout << std::endl;

    // Print player positions on row
    std::cout << "|";
    for (size_t i = 0; i < rowTiles.size(); i++)
    {
        int tileIndex = startIndex + i;
        bool hasPlayer = false;

        std::cout << " ";
        for (auto *player : players)
        {
            if (player && player->getPosition() == tileIndex &&
                player->getStatus() != "BANKRUPT")
            {
                std::cout << player->getUsername().substr(0, 2);
                hasPlayer = true;
                break;
            }
        }
        if (!hasPlayer)
        {
            std::cout << "  ";
        }

        for (int j = (hasPlayer ? 2 : 2); j < 13; j++)
            std::cout << " ";
        std::cout << "|";
    }
    std::cout << std::endl;
}