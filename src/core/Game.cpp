#include "Game.hpp"
#include <iostream>

Game::Game()
    : gameActive(false),
      currentPhase(GamePhase::ROLL),
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

Game::~Game()
{
    std::cout << "[Game] Destructor" << std::endl;
}

void Game::initialize()
{
    std::cout << "[Game] initialize()" << std::endl;
    // TODO: Implementation
}

void Game::initializeFromSave(const std::string currentSaveFile)
{
    std::cout << "[Game] initializeFromSave() with filename: " << currentSaveFile << std::endl;
    // TODO: Implementation
}

bool Game::isGameActive()
{
    std::cout << "[Game] isGameActive(), returning: " << gameActive << std::endl;
    return gameActive;
}

GamePhase Game::getCurrentPhase()
{
    std::cout << "[Game] getCurrentPhase()" << std::endl;
    return currentPhase;
}

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

void Game::useSkillCard(int &cardIndex)
{
    std::cout << "[Game] useSkillCard() with card index: " << cardIndex << std::endl;
    // TODO: Implementation
}

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

CLIRenderer &Game::getRenderer()
{
    std::cout << "[Game] getRenderer()" << std::endl;
    return *renderer;
}

void Game::setGameActive(bool active)
{
    std::cout << "[Game] setGameActive() with value: " << active << std::endl;
    gameActive = active;
}

std::string Game::getCurrentPlayerBalance()
{
    std::cout << "[Game] getCurrentPlayerBalance() string version" << std::endl;
    // TODO: Implementation - return balance as string
    return "";
}

int Game::getCurrentPlayerBalance()
{
    std::cout << "[Game] getCurrentPlayerBalance() int version" << std::endl;
    // TODO: Implementation - return balance as int
    return 0;
}

Player *Game::getCurrentPlayer()
{
    std::cout << "[Game] getCurrentPlayer()" << std::endl;
    // TODO: Implementation - return current player pointer
    return nullptr;
}

Board *Game::getBoard()
{
    std::cout << "[Game] getBoard()" << std::endl;
    return board.get();
}

TransactionLogger *Game::getLogger()
{
    std::cout << "[Game] getLogger()" << std::endl;
    return logger.get();
}

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