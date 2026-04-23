#include "core/BankruptcyManager.hpp"
#include "core/IGameAction.hpp"
#include "models/Player.hpp"
#include "models/PropertyTile.hpp"
#include "models/Board.hpp"
#include <algorithm>


int BankruptcyManager::getSellValue(const PropertyTile& prop) const {
    int value = prop.getPrice();
    const StreetTile* street = dynamic_cast<const StreetTile*>(&prop);
    if (street && street->getBuildingLevel() > 0) {
        value += street->getBuildingSaleValue();
    }
    return value;
}

bool BankruptcyManager::isMortgageable(const PropertyTile& prop, const IGameAction& game) const {
    if (prop.isMortgaged()) return false;

    const StreetTile* street = dynamic_cast<const StreetTile*>(&prop);
    if (!street) return true;

    const auto& group = game.getBoard().getPropertiesByColor(street->getColorGroup());
    for (auto p : group) {
        const StreetTile* s = dynamic_cast<const StreetTile*>(p);
        if (s && s->getBuildingLevel() > 0) return false;
    }
    return true;
}

int BankruptcyManager::calculateMaxLiquidation(const Player& debtor, const IGameAction& game) const {
    int total = debtor.getCash();

    for (auto prop : debtor.getOwnedProperties()) {
        if (prop->isMortgaged()) continue;

        int sellVal = getSellValue(*prop);
        int mortVal = isMortgageable(*prop, game) ? prop->getMortgageValue() : 0;

        total += std::max(sellVal, mortVal);
    }
    return total;
}

bool BankruptcyManager::canCoverWithLiquidation(const Player& debtor, int obligation, const IGameAction& game) const {
    return calculateMaxLiquidation(debtor, game) >= obligation;
}

std::vector<LiquidationOption> BankruptcyManager::getLiquidationOptions(const Player& debtor, const IGameAction& game) const {
    std::vector<LiquidationOption> options;

    for (auto prop : debtor.getOwnedProperties()) {
        if (prop->isMortgaged()) continue;

        int sellVal = getSellValue(*prop);
        options.push_back({
            LiquidationOption::Type::SELL_PROPERTY,
            prop,
            sellVal,
            "Jual " + prop->getName() + " → M" + std::to_string(sellVal)
        });

        if (isMortgageable(*prop, game)) {
            int mortVal = prop->getMortgageValue();
            options.push_back({
                LiquidationOption::Type::MORTGAGE_PROPERTY,
                prop,
                mortVal,
                "Gadai " + prop->getName() + " → M" + std::to_string(mortVal)
            });
        }
    }

    return options;
}

void BankruptcyManager::executeLiquidation(Player& debtor, const LiquidationOption& option, IGameAction& game) {
    auto prop = option.property;

    if (option.type == LiquidationOption::Type::SELL_PROPERTY) {
        if (auto street = dynamic_cast<StreetTile*>(prop)) {
            street->resetBuildings();
        }

        debtor.removeProperty(prop);
        prop->changeOwner(nullptr);
        debtor += option.amount;

        if (game.getLogger())
            game.getLogger()->addLog(debtor.getUsername(), "JUAL", prop->getName());
    }
    else {
        prop->mortgage();
        debtor += option.amount;

        if (game.getLogger())
            game.getLogger()->addLog(debtor.getUsername(), "GADAI", prop->getName());
    }
}

void BankruptcyManager::declareBankruptcy(Player& debtor, Player* creditor, IGameAction& game) {
    if (creditor) {
        *creditor += debtor.getCash();
        debtor.setCash(0);

        for (auto prop : debtor.getOwnedProperties()) {
            prop->changeOwner(creditor);
            creditor->addProperty(prop);
        }

        debtor.clearProperties();
        debtor.clearSkillCards();
    }
    else {
        debtor.setCash(0);

        auto props = debtor.releaseAllProperties();
        debtor.clearSkillCards();

        for (auto prop : props) {
            if (auto street = dynamic_cast<StreetTile*>(prop)) {
                street->resetBuildings();
            }
            prop->changeOwner(nullptr);
            game.triggerAuction(*prop);
        }
    }

    debtor.setStatus("BANKRUPT");

    if (game.countActivePlayers() <= 1) {
        game.setGameActive(false);
    }
}


void BankruptcyManager::handle(Player& debtor, Player* creditor, int obligation, IGameAction& game, std::function<int(const std::vector<LiquidationOption>&)> chooseOption) {
    if (debtor.getCash() >= obligation) {
        debtor -= obligation;
        if (creditor) *creditor += obligation;
        return;
    }

    if (!canCoverWithLiquidation(debtor, obligation, game)) {
        declareBankruptcy(debtor, creditor, game);
        return;
    }

    while (debtor.getCash() < obligation) {
        auto options = getLiquidationOptions(debtor, game);
        if (options.empty()) {
            declareBankruptcy(debtor, creditor, game);
            return;
        }

        int choice = chooseOption(options);

        if (choice < 0 || choice >= (int)options.size()) continue;

        executeLiquidation(debtor, options[choice], game);
    }

    debtor -= obligation;
    if (creditor) *creditor += obligation;

    if (game.getLogger())
        game.getLogger()->addLog(debtor.getUsername(), "LUNAS", std::to_string(obligation));
}