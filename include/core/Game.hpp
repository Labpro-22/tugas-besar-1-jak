#pragma once
#include "core/IGameAction.hpp"
#include <memory>
#include <string>
#include <vector>

// Forward declarations
class PropertyTile;
class StreetTile;
class RailroadTile;
class UtilityTile;
class Dice;
class Board;
class Player;
class ActionCard;
class SkillCard;
template <typename T> class CardDeck;
class TransactionLogger;
class BankruptcyManager;
class AuctionManager;
class CLIRenderer;

class Game : public IGameAction
{
private:
    // ===== Attribute =====
    // Game State
    bool gameActive;
    GamePhase currentPhase;
    int currentPlayerIndex;
    int turnsPlayed;
    int maxTurn;
    std::vector<int> turnOrder;

    // Buat nyimpen nilai-nilai config biar ga load berkali-kali
    int goSalary;
    int jailFine;
    int pphFlat;
    int pphPersen;
    int pbmFlat;

    // Auction State
    bool isAuctionActive;
    class PropertyTile *currentAuctionProperty;
    std::vector<bool> auctionPassed;

    // Komponen utama
    std::unique_ptr<Board> board;
    std::vector<std::unique_ptr<Player>> players;
    std::unique_ptr<Dice> dice;
    std::unique_ptr<TransactionLogger> logger;
    std::unique_ptr<BankruptcyManager> bankruptcyManager;
    std::unique_ptr<AuctionManager> auctionManager;
    std::unique_ptr<CardDeck<SkillCard>> skillCardDeck;
    std::unique_ptr<CardDeck<ActionCard>> chanceDeck;
    std::unique_ptr<CardDeck<ActionCard>> generalFundsDeck;

    // Renderer
    CLIRenderer *renderer;

    // Jail State
    int turnsInJailForCurrentPlayer;
    bool waitingForJailChoice;

    // ===== Method =====
    void initializeDecks();
    void initializeConfig();

    // SaveLoad
    std::string currentSaveFile;

    // private helper
    void nextTurn();
    void processTileLanding(Player &player, int tileIndex);
    void applyRent(Player &player, PropertyTile &tile);
    void startAuctionForProperty(PropertyTile &tile);
    void drawSkillCard(Player &player);
    void movePlayer(Player &player, int steps);
    void teleportPlayer(Player &player, int targetTileIndex);

    // Private helper terkait handle landed tiap petak
    void handleStreetLanding(Player& player, StreetTile& tile);
    void handleRailroadLanding(Player& player, RailroadTile& tile);
    void handleUtilityLanding(Player& player, UtilityTile& tile);

    void handleGameEnd();
public:
    // Konstruktor dan destruktor
    Game();
    ~Game() override;

    // Inisialisasi awal
    void initialize();
    void initializeFromSave(const std::string& saveFile);
    void startNewGame(const std::vector<std::string>& playerNames);
    bool isGameActive() const override;
    void setGameActive(bool active) override;

    // Dadu
    void rollDice() override;
    void setDice(int x, int y) override;

    // Property
    void mortgageProperty() override;
    void redeemProperty(const std::string &tileCode) override;
    void buildOnProperty(const std::string &tileCode) override;
    
    // Auction
    void placeBid(int amount) override;
    void passAuction() override;
    void finalizeAuction(Player* winner, PropertyTile* property, int winningBid) override;
    void logAuctionEvent(const std::string& action, const std::string& detail) override;
    void triggerAuction(PropertyTile& property) override;
    
    // SkillCard
    void useSkillCard(int cardIndex) override;
    
    // Jail
    void payJailFine() override;
    void useJailFreeCard() override;
    
    // Player
    Player *getCurrentPlayer() const override;
    std::string getCurrentPlayerName() const override;
    std::vector<Player*> getActivePlayers() const;
    int countActivePlayers() const override;
    
    // Save and load
    void saveGame(const std::string &filename) override;
    void loadGame(const std::string &filename) override;

    // Display
    void printBoard() override;
    void printDeed(const std::string &tileCode) override;
    void printPropertyInventory() override;
    void printPlayerStatus() override;
    void printLog(int limit) override;
    void printHelp() override;
    void setRenderer(CLIRenderer* r);

    // Flow Giliran
    void endTurn() override;
    void declareBankruptcy() override;

    // Sewa
    void processTileLandingPublic(Player& player, int tileIndex);

    // Helper
    Board &getBoard() const override;
    TransactionLogger *getLogger() override;
};