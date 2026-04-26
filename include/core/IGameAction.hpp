#pragma once

#include <string>
#include <vector>

class Board;
class Player;
class PropertyTile;
class TransactionLogger;

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

    virtual std::vector<Player*> getActivePlayers() const = 0;
    virtual void processTileLandingPublic(Player& player, int tileIndex) = 0;

    virtual void setGameActive(bool active) = 0;
    virtual bool isGameActive() const = 0;
    virtual Board &getBoard() const = 0;
    virtual Player *getCurrentPlayer() const = 0;

    // Dadu
    virtual void rollDice() = 0;
    virtual void setDice(int x, int y) = 0;

    // Property
    virtual void mortgageProperty() = 0;
    virtual void redeemProperty(const std::string &tileCode) = 0;
    virtual void buildOnProperty(const std::string &tileCode) = 0;

    // Auction
    virtual void placeBid(int amount) = 0;
    virtual void passAuction() = 0;
    virtual void finalizeAuction(Player* winner, PropertyTile* property, int winningBid) = 0;
    virtual void logAuctionEvent(const std::string& action, const std::string& detail) = 0;
    virtual void triggerAuction(PropertyTile& property) = 0;

    // SkillCard
    virtual void useSkillCard(int cardIndex) = 0;
    virtual void applyMoveCard(int steps) = 0;
    virtual void applyTeleportCard() = 0;
    virtual void applyDiscountCard(int percent) = 0;
    virtual void applyShieldCard() = 0;
    virtual void applyLassoCard() = 0;
    virtual void applyDemolitionCard() = 0;

    // Jail
    virtual void payJailFine() = 0;
    virtual void useJailFreeCard() = 0;

    // SaveLoad
    virtual void saveGame(const std::string &filename) = 0;
    virtual void loadGame(const std::string &filename) = 0;

    // Display
    virtual void printBoard() = 0;
    virtual void printDeed(const std::string& tileCode) = 0;
    virtual void printPropertyInventory() = 0;
    virtual void printLog(int limit) = 0;
    virtual void printHelp() = 0;
    virtual void printPlayerStatus() = 0;

    // Logger
    virtual TransactionLogger *getLogger() = 0;

    // Player
    virtual int countActivePlayers() const = 0;
    virtual std::string getCurrentPlayerName() const = 0;

    // Game Selesai
    virtual void endTurn() = 0;
    virtual void declareBankruptcy() = 0;
};