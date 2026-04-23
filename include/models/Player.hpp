#pragma once

#include <string>
#include <vector>

class Property;
class SkillCard;
class Game;

class Player
{
private:
    std::string username;
    int position;
    std::string status;
    int cash;
    std::vector<Property *> ownedProperties;
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
    int getMaxLiquidation() const;

    // Card management
    void addCard(SkillCard *card);
    void removeCard(int index);
    void useSkillCard(int index);

    // Property management
    void addProperty(Property *prop);
    void buyProperty(Property *property, Game &game);
    void payRent(int amount, Player &landlord);

    // Game actions
    void goToJail();
    void serveJailTurn();
    void releaseFromJail();
    void resetTurnFlags();

    // Bankruptcy handling
    void handleBankruptcy(Player &creditor, int amountOwed);
    void liquidateAssets(int targetAmount);

    // Getters
    std::string getUsername() const;
    int getPosition() const;
    void setPosition(int pos);
    std::string getStatus() const;
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
    const std::vector<Property *> &getOwnedProperties() const;
    const std::vector<SkillCard *> &getOwnedSkillCards() const;
};
