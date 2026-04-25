#include "models/PropertyTile.hpp"
#include "utils/NimonspoliException.hpp"

// PropertyTile

PropertyTile::PropertyTile(int idx, std::string cd, std::string nm, int bp, int mv)
    : Tile(idx, cd, nm), buyPrice(bp), mortgageValue(mv), owner(nullptr), status(PropertyStatus::BANK),
    festivalMultiplier(1), festivalDuration(0) {}

void PropertyTile::mortgage() {
    if (status != PropertyStatus::OWNED) {
        throw NimonspoliException("Mortgage Property: properti " + this->name + " tidak berstatus OWNED.");
    }
    resetBuildings();
    status = PropertyStatus::MORTGAGED;
}

void PropertyTile::redeem() {
    if (owner == nullptr) {
        throw NimonspoliException("Redeem Property: properti " + this->name + " belum memiliki owner.");
    }
    status = PropertyStatus::OWNED;
}

Player* PropertyTile::getOwner() const { 
    return owner; 
}

bool PropertyTile::isMortgaged() const { 
    return status == PropertyStatus::MORTGAGED; 
}

bool PropertyTile::isOwned() const { 
    return status == PropertyStatus::OWNED && owner != nullptr && !isMortgaged(); 
}

int PropertyTile::getPrice() const { 
    return buyPrice; 
}

int PropertyTile::getMortgageValue() const { 
    return mortgageValue; 
}

PropertyStatus PropertyTile::getStatus() const {
    return status; 
}

PropertyType PropertyTile::getPropertyType() const { 
    return PropertyType::UNDEFINED; 
}

TileType PropertyTile::getTileType() const { 
    return TileType::PROPERTY; 
}

void PropertyTile::changeOwner(Player *newOwner) {
    owner = newOwner;
}

void PropertyTile::applyFestival() {
    if (!isOwned()) {
        throw NimonspoliException("Apply Festival: properti " + this->name + " tidak dimiliki siapa pun.");
    }
    if (festivalMultiplier < 8) {
        festivalMultiplier = festivalMultiplier * 2;
    }
    festivalDuration = 3;
}

void PropertyTile::tickFestival() {
    if (festivalDuration > 0) {
        festivalDuration--;
    }
    if (festivalDuration == 0) {
        festivalMultiplier = 1;
    }
}

bool PropertyTile::isFestivalActive() {
    return festivalDuration > 0;
}

bool PropertyTile::isFestivalMax() {
    return isFestivalActive() && festivalMultiplier >= 8;
}

int PropertyTile::getFestivalMultiplier() const { return festivalMultiplier; }
int PropertyTile::getFestivalDuration() const { return festivalDuration; }
int PropertyTile::getTotalBuildingPrice() { return 0; }

void PropertyTile::resetBuildings() {
    festivalDuration = 0;
    festivalMultiplier = 1;
}

// StreetTile 

StreetTile::StreetTile(int idx, std::string cd, std::string nm, int bp, int mv, std::string cg,
                       std::vector<int> rnt, int hc, int htc)
    : PropertyTile(idx, cd, nm, bp, mv), colorGroup(cg), rents(rnt), buildingLevel(0), 
    houseCost(hc), hotelCost(htc) {}

int StreetTile::calculateRent(int diceTotal, bool isMonopolized) {
    if (!isOwned()) return 0;
    if (rents.empty()) return 0;
    if (buildingLevel < 0 || buildingLevel > 5) {
        throw NimonspoliException("Rent Calculation: building level " + this->name + " melewati batas.");
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

void StreetTile::onLanded(Player& player, Game& game) {
    game.addLog(
        player.getUsername(), "GERAK", "Mendarat di petak Lahan: " + name + "."
    );
    if (status == PropertyStatus::BANK) {
        game.getCLIRenderer()->printInfo(player.getUsername() + " mendarat di petak Lahan: " + name + ".");
        game.getCLIRenderer()->printDeed(*this);
        game.getCLIRenderer()->printInfo("Uang saat ini: " + game.getCLIRenderer()->formatMoney(player.getCash()));
        std::string input = game.getCLIRenderer()->printYesOrNoChoice(
            "Apakah kamu ingin membeli properti ini seharga " + 
            game.getCLIRenderer()->formatMoney(buyPrice) + "? (y/n): " 
        );
        if (input == "y" || input == "Y") {
            if (player.getCash() < buyPrice) {
                game.getCLIRenderer()->printInfo("Uang tidak cukup! Properti masuk ke sistem lelang...");
                game.triggerAuction(*this);
                return;
            }
            player.buyProperty(this, game);
            game.addLog(
                player.getUsername(), "STREET", 
                "Beli lahan " + name + " (" + code + ") seharga " + 
                game.getCLIRenderer()->formatMoney(buyPrice)
            );
            game.getCLIRenderer()->printInfo(name + " kini menjadi milik " + player.getUsername() + "!");
            game.getCLIRenderer()->printInfo("Uang tersisa: " + game.getCLIRenderer()->formatMoney(player.getCash()));
        } else {
            game.getCLIRenderer()->printInfo("Properti ini akan masuk ke sistem lelang...");
            game.triggerAuction(*this);
        }
        return;
    }

    if (owner == &player) {
        if (isMortgaged()) {
            game.getCLIRenderer()->printInfo(player.getUsername() + " mendarat di lahan miliknya sendiri: " + name + " (sedang digadaikan).");
        } else {
            game.getCLIRenderer()->printInfo(player.getUsername() + " mendarat di lahan miliknya sendiri: " + name);
        }
        return;
    }

    if (isMortgaged()) {
        game.getCLIRenderer()->printInfo(player.getUsername() +  " mendarat di petak Lahan: " + name + " (" + code + "), milik " + owner->getUsername() + ".");
        game.getCLIRenderer()->printInfo("Lahan ini sedang digadaikan. Tidak ada sewa yang dikenakan.");
        return;
    }

    if (isOwned() && owner != &player) {
        if (player.isShieldActive()) {
            game.getCLIRenderer()->printInfo(
                "Perlindungan aktif, " + player.getUsername() + " tidak perlu membayar sewa."
            );

            game.addLog(
                player.getUsername(),
                "STREET",
                "Bebas bayar sewa lahan " + name + " karena efek perlindungan."
            );

            return;
        }
        int rentAmount = calculateRent(0, game.getBoard().isMonopolized(colorGroup));
        int payableAmount = player.calculatePayableRent(rentAmount);
        game.getCLIRenderer()->printReceipt(
            "", {}, {
                {
                    "Kondisi", (isMaxLevel()) ? std::to_string(buildingLevel - 1) + " rumah dan " + " 1 hotel" :
                    std::to_string(buildingLevel) + " rumah"
                }, {"Sewa", game.getCLIRenderer()->formatMoney(rentAmount)}
            }
        );
        if (player.getCash() - payableAmount >= 0) {
            int cashBeforeMe = player.getCash();
            int cashBeforeOwner = owner->getCash();
            if (payableAmount != rentAmount) {
                game.getCLIRenderer()->printInfo(
                    "Diskon aktif: " +
                    game.getCLIRenderer()->formatMoney(rentAmount) +
                    " -> " +
                    game.getCLIRenderer()->formatMoney(payableAmount)
                );
            }
            player.payRent(rentAmount, *owner);
            game.getCLIRenderer()->printReceipt(
                "", {}, {
                    {
                        "Uang " + player.getUsername() + " (saya)", game.getCLIRenderer()->formatMoney(cashBeforeMe) +
                        " -> " + game.getCLIRenderer()->formatMoney(player.getCash())
                    }, 
                    {
                        "Uang " + owner->getUsername() + " (pemilik)", game.getCLIRenderer()->formatMoney(cashBeforeOwner) +
                        " -> " + game.getCLIRenderer()->formatMoney(owner->getCash())
                    }
                }
            );
            game.addLog(
                player.getUsername(), "STREET", "Membayar sewa lahan " + name + " senilai " + 
                game.getCLIRenderer()->formatMoney(rentAmount) + "."
            );
        } else {
            game.getCLIRenderer()->printInfo(
                player.getUsername() + " tidak bisa bayar sewa."
            );
            game.getCLIRenderer()->printInfo(
                "Uang sekarang: " + game.getCLIRenderer()->formatMoney(player.getCash())
            );
            // todo: handle bankruptcy, klao liqudiadsi bisa bayar, bayar, kalo nggak bangkrut dengan owner sebagai kreditor
        }
        return;
    }
}

bool StreetTile::canBuild(bool isMonopolized) {
    if (!isOwned()) return false;
    if (!isMonopolized) return false;
    if (buildingLevel >= 5) return false;

    std::vector<StreetTile*> propGroup;
    for (PropertyTile* property : owner->getOwnedProperties()) {
        StreetTile* st = dynamic_cast<StreetTile*>(property);
        if (st && st->getColorGroup() == colorGroup && !st->isMortgaged()) {
            propGroup.push_back(st);
        }
    }
    if (propGroup.empty()) return false;
    int minLevel = propGroup[0]->getBuildingLevel();
    std::for_each(propGroup.begin(), propGroup.end(), [&minLevel](StreetTile* st) {
        if (st->getBuildingLevel() < minLevel) {
            minLevel = st->getBuildingLevel();
        }
    });
    return buildingLevel == minLevel;
}

void StreetTile::build(bool isMonopolized) {
    if (!canBuild(isMonopolized)) {
        throw NimonspoliException("Build Property: properti " + this->name + " belum memenuhi syarat bangun.");
    }
    buildingLevel++;
}

void StreetTile::sell() {
    if (!isOwned()) {
        throw NimonspoliException("Sell Property: properti " + this->name + " tidak dimiliki siapa pun.");
    }
    resetBuildings();
    owner = nullptr;
    status = PropertyStatus::BANK;
}

void StreetTile::setFestivalMultiplier(int mult) { 
    festivalMultiplier = std::max(1, mult); 
}
void StreetTile::setFestivalDuration(int dur) { 
    festivalDuration = std::max(0, dur); 
}
void StreetTile::setBuildingLevel(int level) { 
    buildingLevel = std::max(0, std::min(level, 5)); 
}
bool StreetTile::isMaxLevel() const {
    return buildingLevel >= 5;
}
std::string StreetTile::getColorGroup() const { return colorGroup; }
std::string StreetTile::getDisplayColor() {
    return colorGroup;
}
const std::vector<int> StreetTile::getRents() const { return rents; }
int StreetTile::getHouseCost() const { return houseCost; }
int StreetTile::getHotelCost() const { return hotelCost; }
int StreetTile::getBuildingLevel() const { return buildingLevel; }
PropertyType StreetTile::getPropertyType() const { return PropertyType::STREET; }
int StreetTile::getTotalBuildingPrice() { 
    if (buildingLevel <= 0) return 0;
    if (buildingLevel == 5) return houseCost * 4 + hotelCost;
    return houseCost * buildingLevel; 
}

int StreetTile::getBuildingSaleValue() const {
    if (buildingLevel <= 0) return 0;
    if (buildingLevel == 5) return (houseCost / 2) * 4 + (hotelCost / 2);
    return (houseCost / 2) * buildingLevel;
}

void StreetTile::resetBuildings() { 
    buildingLevel = 0;
    festivalDuration = 0;
    festivalMultiplier = 1;
}

// RailroadTile

RailroadTile::RailroadTile(int idx, std::string cd, std::string nm, int bp, int mv, std::map<int, int> rt)
    : PropertyTile(idx, cd, nm, bp, mv), rentTable(rt) {}

int RailroadTile::calculateRent(int diceTotal, bool isMonopolized) {
    if (!isOwned()) return 0;

    int count = owner->getRailroadCount();
    auto it = rentTable.find(count);
    if (it == rentTable.end()) return 0;
    int mult = isFestivalActive() ? festivalMultiplier : 1;
    return it->second * mult;
}

void RailroadTile::onLanded(Player& player, Game& game) {
    game.addLog(
        player.getUsername(), "GERAK", "Mendarat di petak Stasiun: " + name + "."
    );
    if (status == PropertyStatus::BANK) {
        game.getCLIRenderer()->printInfo(player.getUsername() + " mendarat di petak Stasiun: " + name + "!");
        changeOwner(&player);
        redeem();
        player.addProperty(this);
        game.getCLIRenderer()->printInfo("Belum ada yang menginjaknya duluan, stasiun ini kini menjadi milik " + player.getUsername());
        game.addLog(player.getUsername(), "RAILROAD", "Stasiun " + name + " (" + code + ") kini milik " + player.getUsername() + " (otomatis).");
        return;
    }

    if (owner == &player) {
        if (isMortgaged()) {
            game.getCLIRenderer()->printInfo(player.getUsername() + " mendarat di stasiun miliknya sendiri: " + name + " (sedang digadaikan).");
        } else {
            game.getCLIRenderer()->printInfo(player.getUsername() + " mendarat di stasiun miliknya sendiri: " + name);
        }
        return;
    }

    if (isMortgaged()) {
        game.getCLIRenderer()->printInfo(player.getUsername() +  " mendarat di petak Stasiun: " + name + " (" + code + "), milik " + owner->getUsername() + ".");
        game.getCLIRenderer()->printInfo("Stasiun ini sedang digadaikan. Tidak ada sewa yang dikenakan.");
        return;
    }

    if (isOwned() && owner != &player) {
        if (player.isShieldActive()) {
            game.getCLIRenderer()->printInfo(
                "Perlindungan aktif, " + player.getUsername() + " tidak perlu membayar sewa."
            );

            game.addLog(
                player.getUsername(),
                "RAILROAD",
                "Bebas bayar sewa stasiun " + name + " karena efek perlindungan."
            );

            return;
        }
        int rentAmount = calculateRent();
        int payableAmount = player.calculatePayableRent(rentAmount);
        game.getCLIRenderer()->printReceipt(
            "", {}, {
                {"Sewa", game.getCLIRenderer()->formatMoney(rentAmount)}
            }
        );
        if (player.getCash() - payableAmount >= 0) {
            int cashBeforeMe = player.getCash();
            int cashBeforeOwner = owner->getCash();
            if (payableAmount != rentAmount) {
                game.getCLIRenderer()->printInfo(
                    "Diskon aktif: " +
                    game.getCLIRenderer()->formatMoney(rentAmount) +
                    " -> " +
                    game.getCLIRenderer()->formatMoney(payableAmount)
                );
            }
            player.payRent(rentAmount, *owner);
            game.getCLIRenderer()->printReceipt(
                "", {}, {
                    {
                        "Uang " + player.getUsername() + " (saya)", game.getCLIRenderer()->formatMoney(cashBeforeMe) +
                        " -> " + game.getCLIRenderer()->formatMoney(player.getCash())
                    }, 
                    {
                        "Uang " + owner->getUsername() + " (pemilik)", game.getCLIRenderer()->formatMoney(cashBeforeOwner) +
                        " -> " + game.getCLIRenderer()->formatMoney(owner->getCash())
                    }
                }
            );
            game.addLog(
                player.getUsername(), "RAILROAD", "Membayar sewa stasiun " + name + " senilai " + 
                game.getCLIRenderer()->formatMoney(rentAmount) + "."
            );
        } else {
            game.getCLIRenderer()->printInfo(
                player.getUsername() + " tidak bisa bayar sewa."
            );
            game.getCLIRenderer()->printInfo(
                "Uang sekarang: " + game.getCLIRenderer()->formatMoney(player.getCash())
            );
            // todo: handle bankruptcy, klao liqudiadsi bisa bayar, bayar, kalo nggak bangkrut dengan owner sebagai kreditor
        }
        return;
    }
}

PropertyType RailroadTile::getPropertyType() const { return PropertyType::RAILROAD; }

const std::map<int, int>& RailroadTile::getRentTable() const {
    return rentTable;
}

// UtilityTile

UtilityTile::UtilityTile(int idx, std::string cd, std::string nm, int bp, int mv, std::map<int, int> mt)
    : PropertyTile(idx, cd, nm, bp, mv), multiplierTable(mt) {}

int UtilityTile::calculateRent(int diceTotal, bool isMonopolized) {
    if (status != PropertyStatus::OWNED || owner == nullptr || isMortgaged()) return 0;

    int count = owner->getUtilityCount();
    auto it = multiplierTable.find(count);
    if (it == multiplierTable.end()) return 0;
    int mult = isFestivalActive() ? festivalMultiplier : 1;
    return it->second * diceTotal * mult;
}

void UtilityTile::onLanded(Player& player, Game& game) {
    game.addLog(
        player.getUsername(), "GERAK", "Mendarat di petak Utility: " + name + "."
    );
    if (status == PropertyStatus::BANK) {
        game.getCLIRenderer()->printInfo(player.getUsername() + " mendarat di petak Utility: " + name + "!");
        changeOwner(&player);
        redeem();
        player.addProperty(this);
        game.getCLIRenderer()->printInfo("Belum ada yang menginjaknya duluan, utility ini kini menjadi milik " + player.getUsername());
        game.addLog(player.getUsername(), "UTILITY", "Utility " + name + " (" + code + ") kini milik " + player.getUsername() + " (otomatis).");
        return;
    }

    if (owner == &player) {
        if (isMortgaged()) {
            game.getCLIRenderer()->printInfo(player.getUsername() + " mendarat di utility miliknya sendiri: " + name + " (sedang digadaikan).");
        } else {
            game.getCLIRenderer()->printInfo(player.getUsername() + " mendarat di utility miliknya sendiri: " + name);
        }
        return;
    }

    if (isMortgaged()) {
        game.getCLIRenderer()->printInfo(player.getUsername() +  " mendarat di petak Utility: " + name + " (" + code + "), milik " + owner->getUsername() + ".");
        game.getCLIRenderer()->printInfo("Utility ini sedang digadaikan. Tidak ada sewa yang dikenakan.");
        return;
    }

    if (isOwned() && owner != &player) {
        if (player.isShieldActive()) {
            game.getCLIRenderer()->printInfo(
                "Perlindungan aktif, " + player.getUsername() + " tidak perlu membayar sewa."
            );

            game.addLog(
                player.getUsername(),
                "UTILITY",
                "Bebas bayar sewa utility " + name + " karena efek perlindungan."
            );

            return;
        }
        int rentAmount = calculateRent(game.getDiceTotal());
        int payableAmount = player.calculatePayableRent(rentAmount);
        game.getCLIRenderer()->printReceipt(
            "", {}, {
                {"Sewa", game.getCLIRenderer()->formatMoney(rentAmount)}
            }
        );
        if (player.getCash() - payableAmount >= 0) {
            int cashBeforeMe = player.getCash();
            int cashBeforeOwner = owner->getCash();
            if (payableAmount != rentAmount) {
                game.getCLIRenderer()->printInfo(
                    "Diskon aktif: " +
                    game.getCLIRenderer()->formatMoney(rentAmount) +
                    " -> " +
                    game.getCLIRenderer()->formatMoney(payableAmount)
                );
            }
            player.payRent(rentAmount, *owner);
            game.getCLIRenderer()->printReceipt(
                "", {}, {
                    {
                        "Uang " + player.getUsername() + " (saya)", game.getCLIRenderer()->formatMoney(cashBeforeMe) +
                        " -> " + game.getCLIRenderer()->formatMoney(player.getCash())
                    }, 
                    {
                        "Uang " + owner->getUsername() + " (pemilik)", game.getCLIRenderer()->formatMoney(cashBeforeOwner) +
                        " -> " + game.getCLIRenderer()->formatMoney(owner->getCash())
                    }
                }
            );
            game.addLog(
                player.getUsername(), "UTILITY", "Membayar sewa utility " + name + " senilai " + 
                game.getCLIRenderer()->formatMoney(rentAmount) + "."
            );
        } else {
            game.getCLIRenderer()->printInfo(
                player.getUsername() + " tidak bisa bayar sewa."
            );
            game.getCLIRenderer()->printInfo(
                "Uang sekarang: " + game.getCLIRenderer()->formatMoney(player.getCash())
            );
            // todo: handle bankruptcy, klao liqudiadsi bisa bayar, bayar, kalo nggak bangkrut dengan owner sebagai kreditor
        }
        return;
    }
}

std::string UtilityTile::getDisplayColor() {
    return "ABU_ABU";
}

PropertyType UtilityTile::getPropertyType() const { return PropertyType::UTILITY; }

const std::map<int, int>& UtilityTile::getMultiplierTable() const {
    return multiplierTable;
}
