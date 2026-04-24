#pragma once

#include <string>
#include <vector>
#include "Tile.hpp"

class Board
{
private:
    std::vector<Tile *> tiles;
    bool dynamicBoardEnabled;

public:
    Board();
    ~Board();

    // inisialisasi
    void initializeBoard(const std::string &configFile);
    void setDynamicBoard(bool enabled, int numTiles = 40);

    // Tile
    Tile *getTile(int index) const;
    int getTileCount() const;

    // Board
    void addTile(Tile *tile);
    void removeTile(int index);
    void clearBoard();

    // utility
    bool isMonopolized(std::string colorGroup) const;
    int getStartTileIndex() const;
    int getJailTileIndex() const;
    bool isValidPosition(int index) const;

private:
    // helper inisialisasi
    void validateBoard();
};