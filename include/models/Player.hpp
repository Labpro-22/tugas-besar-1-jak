#pragma once
#include <string>
#include <vector>
#include "models/PropertyTile.hpp"

class SkillCard;
class Game;

class Player
{
private:
    std::string username;
    int position;
    std::string status;
    int cash;
    std::vector<PropertyTile *> ownedProperties;
    std::vector<SkillCard *> ownedSkillCards;
    bool shieldActive;
    int discountPercent;
    int jailTurns;
    int consecutiveDoublesDice;
    bool hasUsedSkillThisTurnVal;

public:
    // Constructor/Destructor
    Player(const std::string &username);
    ~Player();

    // Operator overloads
    Player &operator+=(int amount);
    Player &operator-=(int amount);
    bool operator>(const Player &o) const;
    bool operator<(const Player &o) const;

    // Wealth management
    int getTotalWealth() const;
    int getTotalPropertyPrice() const;
    int getTotalBuildingPrice() const;
    int getMaxLiquidation() const;

    // Card management
    void addCard(SkillCard *card);
    void removeCard(int index);
    void useSkillCard(int index, Game& game);

    // Property management
    void addProperty(PropertyTile *prop);
    void buyProperty(PropertyTile *property, Game &game);
    void payRent(int amount, Player &landlord);
    int calculatePayableRent(int amount);

    // Game actions
    void goToJail(int jailTileIndex);
    void serveJailTurn();
    void releaseFromJail();
    void resetTurnFlags();

    // Bankruptcy handling
    void handleBankruptcy(Player &creditor, int amountOwed);
    void liquidateAssets(int targetAmount);

    void removeProperty(PropertyTile* prop);
    void clearProperties();
    void clearSkillCards();
    std::vector<PropertyTile*> releaseAllProperties();

    // Getters
    std::string getUsername() const;
    int getPosition() const;
    void setPosition(int pos);
    std::string getStatus() const;
    void setStatus(const std::string& status);
    bool isJailed() const;
    int getCash() const;
    void setCash(int amount);
    bool isShieldActive() const;
    void setShieldActive(bool active);
    int getDiscountPercent() const;
    void setDiscountPercent(int percent);
    int getJailTurns() const;
    int getConsecutiveDoublesDice() const;
    void setConsecutiveDoublesDice(int count);
    bool hasUsedSkillThisTurn() const;
    const std::vector<PropertyTile *> &getOwnedProperties() const;
    const std::vector<SkillCard *> &getOwnedSkillCards() const;
    std::vector<std::string> getSkillCardNames() const;
    std::vector<std::string> getSkillCardDescriptions() const;
};