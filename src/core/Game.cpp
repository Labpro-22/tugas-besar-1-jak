#include "Game.hpp"
#include <iostream>

Game::Game()
    : gameActive(false),
      currentPhase(GamePhase::NORMAL),
      currentPlayerIndex(0),
      turnsPlayed(0),
      isAuctionActive(false),
      currentAuctionProperty(nullptr),
      currentHighestBid(0),
      currentHighestBidder(nullptr),
      turnsInJailForCurrentPlayer(0),
      waitingForJailChoice(false),
      renderer(nullptr)
{
    std::cout << "[Game] Constructor" << std::endl;
}

// Inisialisasi
void Game::initialize()
{
    std::cout << "[Game] initialize()" << std::endl;
    // TODO: Implementation
}

void Game::initializeFromSave(const std::string& currentSaveFile)
{
    std::cout << "[Game] initializeFromSave() with filename: " << currentSaveFile << std::endl;
    // TODO: Implementation
}

bool Game::isGameActive() const
{
    std::cout << "[Game] isGameActive(), returning: " << gameActive << std::endl;
    return gameActive;
}

GamePhase Game::getCurrentPhase() const
{
    std::cout << "[Game] getCurrentPhase()" << std::endl;
    return currentPhase;
}

void Game::setGameActive(bool active)
{
    std::cout << "[Game] setGameActive() with value: " << active << std::endl;
    gameActive = active;
}

// Dadu
void Game::rollDice()
{
    std::cout << "[Game] rollDice()" << std::endl;
    // TODO: Implementation
}

void Game::setDice(int x, int y)
{
    std::cout << "[Game] setDice() with values: " << x << ", " << y << std::endl;
    // TODO: Implementation
}

// Property
void Game::buyCurrentProperty()
{
    std::cout << "[Game] buyCurrentProperty()" << std::endl;
    // TODO: Implementation
}

void Game::mortgageProperty(const std::string &code)
{
    std::cout << "[Game] mortgageProperty() for property code: " << code << std::endl;
    // TODO: Implementation
}

void Game::redeemProperty(const std::string &tileCode)
{
    std::cout << "[Game] redeemProperty() for tile code: " << tileCode << std::endl;
    // TODO: Implementation
}

void Game::buildOnProperty(const std::string &tileCode)
{
    std::cout << "[Game] buildOnProperty() for tile code: " << tileCode << std::endl;
    // TODO: Implementation
}

void Game::sellBuildingOnProperty(const std::string &tileCode)
{
    std::cout << "[Game] sellBuildingOnProperty() for tile code: " << tileCode << std::endl;
    // TODO: Implementation
}

// Auction
void Game::placeBid(int amount)
{
    std::cout << "[Game] placeBid() with amount: " << amount << std::endl;
    // TODO: Implementation
}

void Game::passAuction()
{
    std::cout << "[Game] passAuction()" << std::endl;
    // TODO: Implementation
}

// SkillCard
void Game::useSkillCard(int cardIndex)
{
    std::cout << "[Game] useSkillCard() with card index: " << cardIndex << std::endl;
    // TODO: Implementation
}

// Jail
void Game::payJailFine()
{
    std::cout << "[Game] payJailFine()" << std::endl;
    // TODO: Implementation
}

void Game::useJailFreeCard()
{
    std::cout << "[Game] useJailFreeCard()" << std::endl;
    // TODO: Implementation
}

// SaveLoad
void Game::saveGame(const std::string &filename)
{
    std::cout << "[Game] saveGame() with filename: " << filename << std::endl;
    // TODO: Implementation
}

void Game::loadGame(const std::string &filename)
{
    std::cout << "[Game] loadGame() with filename: " << filename << std::endl;
    // TODO: Implementation
}

// Display
void Game::printBoard()
{
    std::cout << "[Game] printBoard()" << std::endl;
    // TODO: Implementation
}

void Game::printDeed(const std::string &tileCode)
{
    std::cout << "[Game] printDeed() for tile code: " << tileCode << std::endl;
    // TODO: Implementation
}

void Game::printPropertyInventory()
{
    std::cout << "[Game] printPropertyInventory()" << std::endl;
    // TODO: Implementation
}

void Game::printLog(int limit)
{
    std::cout << "[Game] printLog() with limit: " << limit << std::endl;
    // TODO: Implementation
}

void Game::printHelp() { 
    std::cout << "[Game] printHelp() - Menampilkan daftar perintah...\n"; 
    // TODO: Implementation
}

void Game::printPlayerStatus() { 
    std::cout << "[Game] printPlayerStatus() - Menampilkan status pemain...\n"; 
    // TODO: Implementation
}

// Flow Giliran
void Game::endTurn() { 
    std::cout << "[Game] endTurn()\n"; 
    // TODO: panggil nextTurn()
}

void Game::declareBankruptcy() { 
    std::cout << "[Game] declareBankruptcy()\n"; 
    // TODO: panggil fungsi di BankruptcyManager
}

// Getter
CLIRenderer &Game::getRenderer()
{
    std::cout << "[Game] getRenderer()" << std::endl;
    return *renderer;
}

int Game::getCurrentPlayerBalance()
{
    std::cout << "[Game] getCurrentPlayerBalance() int version" << std::endl;
    // TODO: Implementation - return balance as int
    return 0;
}

std::string Game::getCurrentPlayerBalanceString()
{
    std::cout << "[Game] getCurrentPlayerBalanceString() " << std::endl;
    // TODO: Implementation - return balance as string
    return "Rp 0"; // Stub
}

Player *Game::getCurrentPlayer() const
{
    std::cout << "[Game] getCurrentPlayer()" << std::endl;
    // TODO: Implementation - return current player pointer
    return nullptr;
}

Board& Game::getBoard() const
{
    std::cout << "[Game] getBoard()" << std::endl;
    return *board;
}

TransactionLogger *Game::getLogger()
{
    std::cout << "[Game] getLogger()" << std::endl;
    return logger.get();
}

// Manajemen Aset Pemain
void Game::addSkillCardToPlayer(Player &player, std::unique_ptr<SkillCard> card)
{
    std::cout << "[Game] addSkillCardToPlayer()" << std::endl;
    // TODO: Implementation
}

void Game::addActionCardToPlayer(Player &player, std::unique_ptr<ActionCard> card)
{
    std::cout << "[Game] addActionCardToPlayer()" << std::endl;
    // TODO: Implementation
}