#include "PropertyTile.hpp"

// PropertyTile

PropertyTile::PropertyTile(int idx, std::string cd, std::string nm, int bp, int mv)
    : Tile(idx, cd, nm), buyPrice(bp), mortgageValue(mv), owner(nullptr), status(PropertyStatus::BANK) {}

void PropertyTile::mortgage() {
    status = PropertyStatus::MORTGAGED;
}

void PropertyTile::redeem() {
    status = PropertyStatus::OWNED;
}

Player* PropertyTile::getOwner() {
    return owner;
}

bool PropertyTile::isMortgaged() { 
    return status == PropertyStatus::MORTGAGED; 
}

int PropertyTile::getPrice()
{
    return buyPrice;
}

void PropertyTile::changeOwner(Player *newOwner)
{
    this->owner = newOwner;
}

// StreetTile

StreetTile::StreetTile(int idx, std::string cd, std::string nm, int bp, int mv, std::string cg, 
    std::vector<int> rnt, int hc, int htc) 
    : PropertyTile(idx, cd, nm, bp, mv), colorGroup(cg), rents(rnt), buildingLevel(0),
      festivalMultiplier(1), festivalDuration(0), houseCost(hc), hotelCost(htc) {}

int StreetTile::calculateRent(int diceTotal) {
    if (status != PropertyStatus::OWNED) {
        return 0;
    }
    int baseRent = rents[buildingLevel];
    if (buildingLevel == 0 && isMonopolized()) {
        baseRent *= 2;
    }
    if (festivalDuration > 0) {
        baseRent *= festivalMultiplier;
    }
    return baseRent;
}

void StreetTile::onLanded(Player& player, Game& game) {}

void StreetTile::build() {
    if (canBuild()) {
        if (buildingLevel < 5) {
            buildingLevel++;
        }
    }
}

void StreetTile::sell() {
    if (status != PropertyStatus::OWNED) {
        return;
    }
    int nilaiGadai = 0;
    if (buildingLevel > 0) {
        if (buildingLevel >= 5) {
            nilaiGadai += (houseCost / 2) * 4 + (hotelCost / 2);
        } else {
            nilaiGadai += (houseCost / 2) * buildingLevel;
        }
    }
    nilaiGadai += mortgageValue;
    mortgage();
}

bool StreetTile::isMonopolized() {
    return false; 
}

bool StreetTile::canBuild() {
    return false;
}

void StreetTile::applyFestival() {
    if (festivalMultiplier < 8) {
        festivalMultiplier *= 2;
    }
    festivalDuration = 3;
}

void StreetTile::tickFestival() {
    if (festivalDuration > 0) {
        festivalDuration--;
    }
    if (festivalDuration == 0) {
        festivalMultiplier = 1;
    }
}

void StreetTile::printDeed() {}

// RailroadTile

RailroadTile::RailroadTile(int idx, std::string cd, std::string nm, int bp, int mv, std::map<int, int> rt) 
    : PropertyTile(idx, cd, nm, bp, mv), rentTable(rt) {}

int RailroadTile::calculateRent(int diceTotal) {
    if (status == PropertyStatus::OWNED) {
        int railloadCount = 0; 
        return rentTable[railloadCount];
    }
    return 0;
}

void RailroadTile::onLanded(Player& player, Game& game) {}

// UtilityTile

UtilityTile::UtilityTile(int idx, std::string cd, std::string nm, int bp, int mv, std::map<int, int> mt) 
    : PropertyTile(idx, cd, nm, bp, mv), multiplierTable(mt) {}

int UtilityTile::calculateRent(int diceTotal) {
    if (status == PropertyStatus::OWNED) {
        int utilityCount = 0; 
        return multiplierTable[utilityCount] * diceTotal;
    }
    return 0;
}

void UtilityTile::onLanded(Player& player, Game& game) {}