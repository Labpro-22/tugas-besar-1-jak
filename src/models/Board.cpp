#include "models/Board.hpp"
#include "models/Tile.hpp"
#include "models/PropertyTile.hpp"
#include "utils/NimonspoliException.hpp"
#include "utils/ConfigLoader.hpp"
#include <iostream>

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
    tiles = ConfigLoader::loadBoard(configFile);
    validateBoard();
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
        if (tiles[i] && (tiles[i]->getCode() == "PEN"))
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

std::vector<PropertyTile*> Board::getPropertiesByColor(const std::string& colorGroup) const {
    std::vector<PropertyTile*> result;
    for (Tile* tile : tiles) {
        StreetTile* st = dynamic_cast<StreetTile*>(tile);
        if (st && st->getColorGroup() == colorGroup) {
            result.push_back(st);
        }
    }
    return result;
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
        if (tile->getCode() == "PEN")
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