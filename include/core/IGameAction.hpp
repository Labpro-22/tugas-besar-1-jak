#pragma once

#include <string>
#include <vector>

class Board;
class Player;

enum class GamePhase
{
    NORMAL,
    ACTION,
    JAIL_CHOICE
};

class IGameAction
{
public:
    virtual ~IGameAction() = default;

    virtual Board &getBoard() const = 0;
    virtual Player *getCurrentPlayer() const = 0;

    // Dadu
    virtual void rollDice() = 0;
    virtual void setDice(int x, int y) = 0;

    // Property
    virtual void buyCurrentProperty() = 0;
    virtual void mortgageProperty(const std::string &code) = 0;
    virtual void redeemProperty(const std::string &tileCode) = 0;
    virtual void buildOnProperty(const std::string &tileCode) = 0;
    virtual void sellBuildingOnProperty(const std::string &tileCode) = 0;

    // Auction
    virtual void placeBid(int amount) = 0;
    virtual void passAuction() = 0;

    // SkillCard
    virtual void useSkillCard(int &cardIndex) = 0;

    // Jail
    virtual void payJailFine() = 0;
    virtual void useJailFreeCard() = 0;

    // SaveLoad
    virtual void saveGame(const std::string &filename) = 0;
    virtual void loadGame(const std::string &filename) = 0;

    // Display
    virtual void printBoard() = 0;
    virtual void printDeed(const std::string &tileCode) = 0;
    virtual void printPropertyInventory() = 0;
    virtual void printLog(int limit) = 0;
    virtual void printHelp() = 0;
    virtual void printPlayerStatus() = 0;

    // Game Selesai
    virtual void endTurn() = 0;
    virtual void declareBankruptcy() = 0;
};