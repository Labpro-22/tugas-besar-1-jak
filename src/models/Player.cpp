#include "Player.hpp"
#include "NimonspoliException.hpp"
#include "Game.hpp"
#include <algorithm>
#include <stdexcept>

class SkillCard;

// Constructor
Player::Player(const std::string &username)
    : username(username), position(0), status("ACTIVE"), cash(1500),
      shieldActive(false), discountPercent(0), jailTurns(0),
      consecutiveDoublesDice(0), hasUsedSkillThisTurnVal(false)
{
    // Iniaialisasi
    ownedProperties = std::vector<PropertyTile *>();
    ownedSkillCards = std::vector<SkillCard *>();
}

// Destructor
Player::~Player()
{

    for (SkillCard *card : ownedSkillCards)
    {
        delete card;
    }
    ownedSkillCards.clear();
}

// operator overloads
Player &Player::operator+=(int amount)
{
    if (amount < 0)
    {
        throw NimonspoliException("[Operator+=] Tidak boleh menggunakan jumlah negatif");
    }
    cash += amount;
    return *this;
}

Player &Player::operator-=(int amount)
{
    if (amount < 0)
    {
        throw NimonspoliException("[Operator-=] Tidak boleh menggunakan jumlah negatif");
    }
    if (cash - amount < 0)
    {
        throw NimonspoliException("[Operator-=] Uang tidak cukup untuk melakukan transaksi");
    }
    cash -= amount;
    return *this;
}

bool Player::operator>(const Player &o) const
{
    return getTotalWealth() > o.getTotalWealth();
}

bool Player::operator<(const Player &o) const
{
    return getTotalWealth() < o.getTotalWealth();
}

// Wealth calculation methods
int Player::getTotalWealth() const
{
    int total = cash;

    // harga total seluruh properti
    for (PropertyTile *prop : ownedProperties)
    {
        if (prop != nullptr)
        {
            total += prop->getPrice();
        }
    }

    return total;
}

int Player::getMaxLiquidation() const
{
    int maxLiquidation = cash;

    // max hasil penjualan semua properti
    const double LIQUIDATION_RATE = 0.7; // 70% harga properti

    for (PropertyTile *prop : ownedProperties)
    {
        if (prop != nullptr)
        {
            maxLiquidation += static_cast<int>(prop->getPrice() * LIQUIDATION_RATE);
        }
    }

    return maxLiquidation;
}

// Card Management
void Player::addCard(SkillCard *card)
{
    if (card == nullptr)
    {
        throw NimonspoliException("[addCard] Kartu tidak boleh null");
    }
    ownedSkillCards.push_back(card);
}

void Player::removeCard(int index)
{
    if (index < 0 || index >= static_cast<int>(ownedSkillCards.size()))
    {
        throw NimonspoliException("[removeCard] Index Kartu tidak valid");
    }

    delete ownedSkillCards[index];
    ownedSkillCards.erase(ownedSkillCards.begin() + index);
}

// Property management methods
void Player::addProperty(PropertyTile *prop)
{
    if (prop == nullptr)
    {
        throw NimonspoliException("[addProperty] Properti tidak boleh null");
    }

    // Cek kepemilikan
    for (PropertyTile *owned : ownedProperties)
    {
        if (owned == prop)
        {
            throw NimonspoliException("[addProperty] Properti sudah dimiliki");
        }
    }

    ownedProperties.push_back(prop);
}

// Helper
void Player::useSkillCard(int index)
{
    if (hasUsedSkillThisTurn)
    {
        throw NimonspoliException("[useSkillCard] Pemain sudah menggunakan skill");
    }

    if (index < 0 || index >= static_cast<int>(ownedSkillCards.size()))
    {
        throw NimonspoliException("[useSkillCard] Index Kartu Skill tidak valid");
    }

    if (ownedSkillCards[index] == nullptr)
    {
        throw NimonspoliException("[useSkillCard] Index Kartu tidak valid");
    }

    // Use
    ownedSkillCards[index]->use(*this);

    // Remove
    removeCard(index);
    hasUsedSkillThisTurnVal = true;
}

void Player::buyProperty(PropertyTile *property, Game &game)
{
    if (property == nullptr)
    {
        throw NimonspoliException("[buyProperty] Properti kosong tidak boleh dibeli");
    }

    if (property->getOwner() != nullptr)
    {
        throw NimonspoliException("[buyProperty] Properti sudah dimiliki");
    }

    int price = property->getPrice();
    if (cash < price)
    {
        throw NimonspoliException("[buyProperty] Uang tidak cukup untuk melakukan transaksi");
    }

    // Mengurangi saldo dan pemindahan kepemilikan
    cash -= price;
    property->changeOwner(this);
    addProperty(property);
}

void Player::payRent(int amount, Player &landlord)
{
    if (amount < 0)
    {
        throw NimonspoliException("[payRent] Harga sewa harus positif");
    }

    // harga diskon, kalau ada
    int discountedAmount = amount;
    if (discountPercent > 0)
    {
        discountedAmount = amount * (100 - discountPercent) / 100;
    }

    if (cash < discountedAmount)
    {
        // harga diskon pun ga mampu
        handleBankruptcy(landlord, discountedAmount);
    }
    else
    {
        cash -= discountedAmount;
        landlord.cash += discountedAmount;
    }
}

void Player::handleBankruptcy(Player &creditor, int amountOwed)
{
    // cairkan semua property
    int totalAvailable = getMaxLiquidation();

    if (totalAvailable < amountOwed)
    {
        // Bangkrut, transfer semua aset ke kreditor
        status = "BANKRUPT";

        // transfer semua properti ke kreditor
        for (PropertyTile *prop : ownedProperties)
        {
            prop->changeOwner(&creditor);
            creditor.addProperty(prop);
        }
        ownedProperties.clear();

        //  transfer semua uang ke kreditor
        creditor.cash += cash;
        cash = 0;

        throw NimonspoliException("[handleBankruptcy] Player " + username + " bangkrut!");
    }
    else
    {
        // liqudasi sebagian aset
        liquidateAssets(amountOwed);
        cash -= amountOwed;
        creditor.cash += amountOwed;
    }
}

void Player::liquidateAssets(int targetAmount)
{
    int amountRaised = 0;

    // Sort properties by value (Lowest Descending)
    std::sort(ownedProperties.begin(), ownedProperties.end(),
              [](PropertyTile *a, PropertyTile *b)
              {
                  return a->getPrice() < b->getPrice();
              });

    const double LIQUIDATION_RATE = 0.7;

    auto it = ownedProperties.begin();
    while (it != ownedProperties.end() && amountRaised < targetAmount)
    {
        int liquidationValue = static_cast<int>((*it)->getPrice() * LIQUIDATION_RATE);
        amountRaised += liquidationValue;
        cash += liquidationValue;

        // hapus kepemilikan
        (*it)->changeOwner(nullptr);
        it = ownedProperties.erase(it);
    }
}

void Player::goToJail()
{
    status = "JAILED";
    jailTurns = 3; // jail duration
    position = 10; // tile index
}

void Player::serveJailTurn()
{
    if (status != "JAILED")
    {
        throw NimonspoliException("[serveJailTurn] Pemain tidak di penjara");
    }

    if (jailTurns > 0)
    {
        jailTurns--;
        if (jailTurns == 0)
        {
            releaseFromJail();
        }
    }
}

void Player::releaseFromJail()
{
    status = "ACTIVE";
    jailTurns = 0;
}

void Player::resetTurnFlags()
{
    hasUsedSkillThisTurnVal = false;
}

// Getters and Setters
std::string Player::getUsername() const
{
    return username;
}
int Player::getPosition() const
{
    return position;
}
void Player::setPosition(int pos)
{
    position = pos;
}
std::string Player::getStatus() const
{
    return status;
}
int Player::getCash() const
{
    return cash;
}
void Player::setCash(int amount)
{
    cash = amount;
}
bool Player::isShieldActive() const
{
    return shieldActive;
}
void Player::setShieldActive(bool active)
{
    shieldActive = active;
}
int Player::getDiscountPercent() const
{
    return discountPercent;
}
void Player::setDiscountPercent(int percent)
{
    discountPercent = percent;
}
int Player::getJailTurns() const
{
    return jailTurns;
}
int Player::getConsecutiveDoublesDice() const
{
    return consecutiveDoublesDice;
}
void Player::setConsecutiveDoublesDice(int count)
{
    consecutiveDoublesDice = count;
}
bool Player::hasUsedSkillThisTurn() const
{
    return hasUsedSkillThisTurnVal;
}
const std::vector<PropertyTile *> &Player::getOwnedProperties() const
{
    return ownedProperties;
}
const std::vector<SkillCard *> &Player::getOwnedSkillCards() const
{
    return ownedSkillCards;
}
