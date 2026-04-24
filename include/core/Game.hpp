#pragma once
#include "core/IGameAction.hpp"
#include "models\Board.hpp"
#include "models\Dice.hpp"
#include "models\Player.hpp"
#include "models\ActionCard.hpp"
#include "models\SkillCard.hpp"
#include "models\CardDeck.hpp"
#include <memory>
#include <string>
#include <vector>

// Forward declarations
class TransactionLogger;
class BankruptcyManager;
class AuctionManager;
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
    Player *currentHighestBidder;
    std::vector<bool> auctionPassed;

    // Komponen utama
    std::unique_ptr<Board> board;
    std::vector<std::unique_ptr<Player>> players;
    std::unique_ptr<Dice> dice;
    std::unique_ptr<TransactionLogger> logger;
    std::unique_ptr<BankruptcyManager> bankruptcyManager;
    std::unique_ptr<AuctionManager> auctionManager;
    std::unique_ptr<CardDeck<ActionCard>> chanceCardDeck;
    std::unique_ptr<CardDeck<ActionCard>> communityChestCardDeck;
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
    void drawSkillCard(Player &player);
    void movePlayer(Player &player, int steps);
    void teleportPlayer(Player &player, int targetTileIndex);
    bool canBuildOnProperty(Player &player, PropertyTile &tile);
    int calculatePropertyValue(PropertyTile &tile);

public:
    Game();
    ~Game() override;

    // Inisialisasi
    void initialize();
    void initializeFromSave(const std::string& currentSaveFile);
    bool isGameActive() const override;
    GamePhase getCurrentPhase() const;
    void setGameActive(bool active) override;

    // Yang bakal dipake sama Command dari IGameAction
    // Dadu
    void rollDice() override;
    void setDice(int x, int y) override;

    // Property
    void buyCurrentProperty() override;
    void mortgageProperty(const std::string &code) override;
    void redeemProperty(const std::string &tileCode) override;
    void buildOnProperty(const std::string &tileCode) override;
    void sellBuildingOnProperty(const std::string &tileCode) override;

    // Auction
    void placeBid(int amount) override;
    void passAuction() override;

    // SkillCard
    void useSkillCard(int cardIndex) override;

    // Jail
    void payJailFine() override;
    void useJailFreeCard() override;

    // SaveLoad
    void saveGame(const std::string &filename) override;
    void loadGame(const std::string &filename) override;

    // Display
    void printBoard() override;
    void printDeed(const std::string &tileCode) override;
    void printPropertyInventory() override;
    void printLog(int limit) override;
    void printHelp() override;
    void printPlayerStatus() override;

    // Flow Giliran
    void endTurn() override;
    void declareBankruptcy() override;

    // Getter
    CLIRenderer &getRenderer();
    int getCurrentPlayerBalance();
    std::string getCurrentPlayerBalanceString(); 
    Player *getCurrentPlayer() const override;
    Board &getBoard() const override;
    TransactionLogger *getLogger();

    // draw kartu
    void drawChanceCard(Player &player);
    void drawCommunityChestCard(Player &player);

    // Manajemen Aset Pemain
    void addSkillCardToPlayer(Player &player, std::unique_ptr<SkillCard> card);
    void addActionCardToPlayer(Player &player, std::unique_ptr<ActionCard> card);
};