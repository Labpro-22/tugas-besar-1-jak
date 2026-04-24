#include "models/Player.hpp"
#include "utils/NimonspoliException.hpp"
#include "core/Game.hpp"
#include "models/SkillCard.hpp"
#include <algorithm>
#include <stdexcept>

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
    for (PropertyTile *prop : ownedProperties)
    {
        if (prop != nullptr)
        {
            total += prop->getPrice();
            total += prop->getTotalBuildingPrice();
        }
    }
    return total;
}

int Player::getTotalPropertyPrice() const
{
    int total = 0;
    for (PropertyTile *prop : ownedProperties)
    {
        if (prop != nullptr)
        {
            total += prop->getPrice();
        }
    }
    return total;
}

int Player::getTotalBuildingPrice() const
{
    int total = 0;

    // harga total seluruh properti
    for (PropertyTile *prop : ownedProperties)
    {
        if (prop != nullptr)
        {
            total += prop->getTotalBuildingPrice();
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
void Player::useSkillCard(int index, Game& game)
{
    if (hasUsedSkillThisTurnVal)
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
    ownedSkillCards[index]->use(*this, game);

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
    property->redeem();
    addProperty(property);
}

void Player::payRent(int amount, Player &landlord)
{
    if (amount < 0)
    {
        return;
    }
    int discountedAmount = calculatePayableRent(amount);
    if (cash < discountedAmount) {
        throw NimonspoliException("[payRent] player tidak bisa bayar sewa");
    } else
    {
        cash -= discountedAmount;
        landlord.cash += discountedAmount;
    }
}

int Player::calculatePayableRent(int amount) {
    if (amount < 0)
    {
        return 0;
    }
    int discountedAmount = amount;
    if (discountPercent > 0)
    {
        discountedAmount = amount * (100 - discountPercent) / 100;
    }
    return discountedAmount;
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

void Player::goToJail(int jailTileIndex)
{
    status = "JAILED";
    jailTurns = 3;
    position = jailTileIndex;
    consecutiveDoublesDice = 0;
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

void Player::removeProperty(PropertyTile* prop) {
    auto it = std::find(ownedProperties.begin(), ownedProperties.end(), prop);
    if (it != ownedProperties.end()) ownedProperties.erase(it);
}

void Player::clearProperties() {
    ownedProperties.clear();
}

void Player::clearSkillCards() {
    for (SkillCard* card : ownedSkillCards) delete card;
    ownedSkillCards.clear();
}

std::vector<PropertyTile*> Player::releaseAllProperties() {
    std::vector<PropertyTile*> released = ownedProperties;
    ownedProperties.clear();
    return released;
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
void Player::setStatus(const std::string& status) { 
    this->status = status; 
}
int Player::getCash() const
{
    return cash;
}
bool Player::isJailed() const
{
    return status == "JAILED";
}
int Player::getUtilityCount() const 
{
    return std::count_if(ownedProperties.begin(), ownedProperties.end(), [](PropertyTile* p) {
        return p->getPropertyType() == PropertyType::UTILITY;
    });
}
int Player::getRailroadCount() const
{
    return std::count_if(ownedProperties.begin(), ownedProperties.end(), [](PropertyTile* p) {
        return p->getPropertyType() == PropertyType::RAILROAD;
    });
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

std::vector<std::string> Player::getSkillCardNames() const {
    std::vector<std::string> names;
    for (SkillCard* card : ownedSkillCards) {
        if (card) names.push_back(card->getName());
    }
    return names;
}

std::vector<std::string> Player::getSkillCardDescriptions() const {
    std::vector<std::string> descs;
    for (SkillCard* card : ownedSkillCards) {
        if (card) descs.push_back(card->getDescription());
    }
    return descs;
}