#pragma once
#include "IGameAction.hpp"
#include <memory>
#include <string>
#include <vector>

class Dice;
class TransactionLogger;
class BankruptcyManager;
class AuctionManager;
class CardDeck;
class ActionCard;
class SkillCard;
class CLIRenderer;

class Game : public IGameAction
{
private:
    // Game State
    bool gameActive;
    GamePhase currentPhase;
    int currentPlayerIndex;
    int turnsPlayed;

    // Auction State
    bool isAuctionActive;
    class PropertyTile *currentAuctionProperty;
    int currentHighestBid;
    class Player *currentHighestBidder;
    std::vector<bool> auctionPassed;

    // core attr
    std::unique_ptr<Board> board;
    std::vector<std::unique_ptr<Player>> players;
    std::unique_ptr<Dice> dice;
    std::unique_ptr<TransactionLogger> logger;
    std::unique_ptr<BankruptcyManager> bankruptcyManager;
    std::unique_ptr<AuctionManager> auctionManager;
    std::unique_ptr<CardDeck<ActionCard>> actionCardDeck;
    std::unique_ptr<CardDeck<SkillCard>> skillCardDeck;

    // Renderer
    CLIRenderer *renderer;

    // Jail State
    int turnsInJailForCurrentPlayer;
    bool waitingForJailChoice;

    // SaveLoad
    std::string currentSaveFile;

    // private helper
    void nextTurn();
    void processTileLanding(Player &player, int tileIndex);
    void applyRent(Player &player, PropertyTile &tile);
    void startAuctionForProperty(PropertyTile &tile);
    void endAuction();
    void checkBankruptcy(Player &player);
    void drawActionCard(Player &player);
    void drawSkillCard(Player &player);
    void movePlayer(Player &player, int steps);
    void teleportPlayer(Player &player, int targetTileIndex);
    bool canBuildOnPropery(Player &player, PropertyTile &tile);
    int calculatePropertyValue(PropertyTile &tile);

public:
    Game();
    ~Game() = default;

    void initialize();
    void initializeFromSave(const std::string currentSaveFile);

    bool isGameActive();
    GamePhase getCurrentPhase();

    void rollDice();
    void setDice(int x, int y);

    // Property
    void buyCurrentProperty();
    void mortgageProperty(const std::string &code);
    void redeemProperty(const std::string &tileCode);
    void buildOnProperty(const std::string &tileCode);
    void sellBuildingOnProperty(const std::string &tileCode);

    // Auction
    void placeBid(int amount);
    void passAuction();

    // SkillCard
    void useSkillCard(int &cardIndex);

    // Jail
    void payJailFine();
    void useJailFreeCard();

    // SaveLoad
    void saveGame(const std::string &filename);
    void loadGame(const std::string &filename);

    // Display
    void printBoard();
    void printDeed(const std::string &tileCode);
    void printPropertyInventory();
    void printLog(int limit);

    CLIRenderer &getRenderer;
    void setGameActive(bool active);

    std::string getCurrentPlayerBalance();
    int getCurrentPlayerBalance();

    Player *getCurrentPlayer();
    Board *getBoard();
    TransactionLogger *getLogger();

    void addSkillCardToPlayer(Player &player, std::unique_ptr<SkillCard> card);
    void addActionCardToPlayer(Player &player, std::unique_ptr<ActionCard> card);
};