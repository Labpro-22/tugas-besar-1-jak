#pragma once
#include "../../include/core/IGameAction.hpp"
#include "../../include/utils/NimonspoliException.hpp"
#include "../../include/models/Board.hpp"
#include "../../include/models/Player.hpp"
#include "../../include/models/Board.hpp"
#include "../../include/models/Dice.hpp"
#include "../../include/models/Tile.hpp"
#include "../../include/models/PropertyTile.hpp"
#include "../../include/models/StreetTile.hpp"
#include "../../include/models/CardDeck.hpp"
#include "../../include/models/SkillCard.hpp"
#include "../../include/models/ActionCard.hpp"
#include "../../include/utils/TransactionLogger.hpp"
#include "../../include/utils/BankruptcyManager.hpp"
#include "../../include/utils/AuctionManager.hpp"
#include "../../include/views/CLIRenderer.hpp"

#include <memory>
#include <string>
#include <vector>

class Game : public IGameAction
{
private:
    // Game State
    bool GameActive;
    GamePhase currentPhase;
    int currentPlayerIndex;
    int turnsPlayed;

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
    void canBuildOnPropery(Player &player, PropertyTile &tile);
    void calculatePropertyValue(PropertyTile &tile);

public:
    Game();
    ~Game() = default;

    void initialize();
    void initializeFromSave(const std::string currentSaveFile);

    bool isGameActive() const override;
    GamePhase getCurrentPhase() const override;

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
    void useSkillCard(int &cardIndex) override;

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

    CLIRenderer &getRenderer override;
    void setGameActive(bool active) override;

    std::string getCurrentPlayerBalance() const override;
    int getCurrentPlayerBalance() const override;

    Player *getCurrentPlayer();
    Board *getBoard();
    TransactionLogger *getLogger();

    void addSkillCardToPlayer(Player &player, std::unique_ptr<SkillCard> card);
    void addActionCardToPlayer(Player &player, std::unique_ptr<ActionCard> card);
};