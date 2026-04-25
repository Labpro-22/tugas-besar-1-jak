#include "models/ActionTile.hpp"

// ActionTile 

ActionTile::ActionTile(int idx, std::string cd, std::string nm)
    : Tile(idx, cd, nm) {}

TileType ActionTile::getTileType() const { return TileType::ACTION; }

// CardTile

CardTile::CardTile(int idx, std::string cd, std::string nm, DeckType dt)
    : ActionTile(idx, cd, nm), deckType(dt) {}

DeckType CardTile::getDeckType() const {
    return deckType;
}

void CardTile::onLanded(Player& player, Game& game) {}
void CardTile::onLanded(Player& player, Game& game) {
    if (deckType == DeckType::CHANCE) {
        game.addLog(player.getUsername(), "GERAK", "Mendarat di petak Kesempatan.");
        game.getCLIRenderer()->printInfo(player.getUsername() + " mendarat di petak Kesempatan.");
        game.drawChanceCard(player);
    } else {
        game.addLog(player.getUsername(), "GERAK", "Mendarat di petak Dana Umum.");
        game.getCLIRenderer()->printInfo(player.getUsername() + " mendarat di petak Dana Umum.");
        game.drawGeneralFundsCard(player);
    }
}

DeckType CardTile::getDeckType() const { return deckType; }

// FestivalTile 

FestivalTile::FestivalTile(int idx, std::string cd, std::string nm)
    : ActionTile(idx, cd, nm) {}

void FestivalTile::onLanded(Player& player, Game& game) {
    game.addLog(player.getUsername(), "GERAK", "Mendarat di petak Festival.");
    game.getCLIRenderer()->printInfo(player.getUsername() + " mendarat di petak Festival.");
    std::vector<PropertyTile*> propertyOwned;
    std::vector<std::string> choiceFormat;
    std::for_each(player.getOwnedProperties().begin(), player.getOwnedProperties().end(), [&propertyOwned, &choiceFormat](PropertyTile* property){
        if (property->isOwned()) {
            propertyOwned.push_back(property);
            choiceFormat.push_back(property->getCode() + " - " + property->getName());
        }
    });
    if (propertyOwned.empty()) {
        game.getCLIRenderer()->printInfo(
            player.getUsername() + " tidak memiliki properti yang tersedia untuk efek festival."
        );
        return;
    }
    int choice = game.getCLIRenderer()->printIntegerChoice(
        "Daftar properti milik " + player.getUsername(),
        choiceFormat,
        "Pilih nomor properti: "
    );
    int index = choice - 1;
    PropertyTile* selected = propertyOwned.at(index);
    bool isFestivalActive = selected->isFestivalActive();
    bool isFestivalMax = selected->isFestivalMax();
    int sewaBefore = selected->calculateRent(game.getDiceTotal(), game.getBoard());
    selected->applyFestival();
    if (!isFestivalActive) {
        game.getCLIRenderer()->printInfo(
            "Efek festival " + selected->getName() + " aktif!\n"
        );
    } else if (!isFestivalMax) {
        game.getCLIRenderer()->printInfo(
            "Efek festival " + selected->getName() + " diperkuat!\n"
        );
    } else {
        game.getCLIRenderer()->printInfo(
            "Efek festival " + selected->getName() + " sudah maksimum!\n"
        );
    }
    if (!isFestivalMax) {
        game.getCLIRenderer()->printInfo(
            "Sewa sebelumnya: " + game.getCLIRenderer()->formatMoney(sewaBefore) + "\n" +
            "Sewa sekarang: " + game.getCLIRenderer()->formatMoney(selected->calculateRent(game.getDiceTotal(), game.getBoard()))
        );
    }
    game.getCLIRenderer()->printInfo(
        ((isFestivalActive) ? "Durasi efek diatur menjadi " : "Durasi efek di-reset menjadi ") +
        std::to_string(selected->getFestivalDuration()) + " giliran."
    );
    if (!isFestivalActive) {
        game.addLog(
            player.getUsername(), "FESTIVAL", "Efek festival properti " + selected->getName() + " aktif."
        );
    } else {
        game.addLog(
            player.getUsername(), "FESTIVAL", "Efek festival properti " + 
            selected->getName() + " diperkuat: x" + std::to_string(selected->getFestivalMultiplier()) + "."
        );
    }
}

// TaxTile

TaxTile::TaxTile(int idx, std::string cd, std::string nm, TaxType txt, int fm, double pct) 
    : ActionTile(idx, cd, nm), taxType(txt), flatAmount(fm), percentage(pct) {}

void TaxTile::onLanded(Player& player, Game& game) {
    int amount = 0; bool bangkrut = false; int choice = -1;
    int cashBefore = player.getCash();
    if (taxType == TaxType::PPH) {
        game.addLog(player.getUsername(), "GERAK", "Mendarat di petak Pajak PPH.");
        game.getCLIRenderer()->printInfo(player.getUsername() + " mendarat di petak Pajak PPH.");
        if (player.isShieldActive()) {
            game.getCLIRenderer()->printInfo(
                "Efek perlindungan aktif, " + player.getUsername() + " terlindungi dari bayar pajak PPH."
            );
            game.addLog(
                player.getUsername(), "PAJAK", "Bebas bayar pajak PPH karena efek perlindungan."
            );
            return;
        }
        choice = game.getCLIRenderer()->printIntegerChoice(
            "Opsi pembayaran pajak:",
            {"Bayar flat " + game.getCLIRenderer()->formatMoney(flatAmount), "Bayar " + std::to_string((int)percentage) 
                + "% dari total kekayaan"},
            "Pilihan nomor: "
        );
        if (choice == 1) {
            amount = flatAmount;
            game.getCLIRenderer()->printInfo(
                "Pajak sebesar " + game.getCLIRenderer()->formatMoney(amount) + " langsung dipotong."
            );
            if (cashBefore - amount >= 0) {
                player -= amount;
                game.addLog(
                    player.getUsername(), "PAJAK", "Bayar pajak PPH flat " +
                    game.getCLIRenderer()->formatMoney(amount) + "."
                );
            } else {
                bangkrut = true;
            }
        } else {
            int wealth = player.getTotalWealth();
            amount = wealth * percentage / 100;
            game.getCLIRenderer()->printReceipt(
            "Total kekayaan " + player.getUsername(),
                {
                    {"Uang tunai", game.getCLIRenderer()->formatMoney(cashBefore)},
                    {"Harga beli properti", game.getCLIRenderer()->formatMoney(player.getTotalPropertyPrice())},
                    {"Harga beli bangunan", game.getCLIRenderer()->formatMoney(player.getTotalBuildingPrice())}
                },
                {
                    {"Total", game.getCLIRenderer()->formatMoney(wealth)},
                    {"Pajak " + std::to_string((int)percentage) + "%", game.getCLIRenderer()->formatMoney(amount)}
                }
            );
            if (cashBefore - amount >= 0) {
                player -= amount;
                game.addLog(
                    player.getUsername(), "PAJAK", "Bayar pajak PPH " + std::to_string((int)percentage) + "% kekayaan " +
                    game.getCLIRenderer()->formatMoney(amount) + "."
                );
            } else {
                bangkrut = true;
            }
        }
    } else {
        game.addLog(player.getUsername(), "GERAK", "Mendarat di petak Pajak PBM.");
        game.getCLIRenderer()->printInfo(player.getUsername() + " mendarat di petak Pajak PBM.");
        if (player.isShieldActive()) {
            game.getCLIRenderer()->printInfo(
                "Efek perlindungan aktif, " + player.getUsername() + " terlindungi dari bayar pajak PBM."
            );
            game.addLog(
                player.getUsername(), "PAJAK", "Bebas bayar pajak PBM karena efek perlindungan."
            );
            return;
        }
        amount = flatAmount;
        game.getCLIRenderer()->printInfo(
            "Pajak sebesar " + game.getCLIRenderer()->formatMoney(amount) + " langsung dipotong."
        );
        if (cashBefore - amount >= 0) {
            player -= amount;
            game.addLog(
                player.getUsername(), "PAJAK", "Bayar pajak PBM flat " +
                game.getCLIRenderer()->formatMoney(amount) + "."
            );
        } else {
            bangkrut = true;
        }
    }
    if (bangkrut) {
        game.getCLIRenderer()->printInfo(
            player.getUsername() + " tidak bisa bayar pajak."
        );
    }
    game.getCLIRenderer()->printInfo(
        "Uang sekarang: " + game.getCLIRenderer()->formatMoney(cashBefore) + " -> " + 
        game.getCLIRenderer()->formatMoney(player.getCash())
    );
    if (bangkrut) {
        // Todo: handle bangkrut, kalo liquidasi masih bisa bayar: bayar pajak, kalo nggak, bangkrut dengan bank sebagi kreditor
    }
}

TaxType TaxTile::getTaxType() const { return taxType; }
