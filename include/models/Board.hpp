#pragma once

#include <string>
#include <vector>

class Tile;
class Player;

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

    // print board
    void printBoard(const std::vector<Player *> &players) const;

    // Board
    void addTile(Tile *tile);
    void removeTile(int index);
    void clearBoard();

    // utility
    int getStartTileIndex() const;
    int getJailTileIndex() const;
    bool isValidPosition(int index) const;

private:
    // helper inisialisasi
    void loadTilesFromConfig(const std::string &configFile);
    void validateBoard();

    // helper print
    std::string getTileColorCode(Tile *tile) const;
    std::string getOwnershipLegend(const std::vector<Player *> &players) const;
    void printTopBorder() const;
    void printBottomBorder() const;
    void printTileRow(const std::vector<Tile *> &rowTiles, const std::vector<Player *> &players, int startIndex) const;
};