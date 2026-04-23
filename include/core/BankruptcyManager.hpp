#pragma once
#include <string>
#include <vector>
#include <functional>

class Player;
class IGameAction;
class TransactionLogger;
class PropertyTile;

struct LiquidationOption {
    enum class Type { SELL_PROPERTY, MORTGAGE_PROPERTY };
    Type type;
    PropertyTile* property;
    int amount;
    std::string description;
};

class BankruptcyManager {
public:
    void handle(Player& debtor, Player* creditor, int obligation, IGameAction& game, std::function<int(const std::vector<LiquidationOption>&)> chooseOption);

private:
    int getSellValue(const PropertyTile& prop) const;
    bool isMortgageable(const PropertyTile& prop, const IGameAction& game) const;

    int calculateMaxLiquidation(const Player& debtor, const IGameAction& game) const;
    bool canCoverWithLiquidation(const Player& debtor, int obligation, const IGameAction& game) const;

    std::vector<LiquidationOption> getLiquidationOptions(const Player& debtor, const IGameAction& game) const;

    void executeLiquidation(Player& debtor, const LiquidationOption& option, IGameAction& game);

    void declareBankruptcy(Player& debtor, Player* creditor, IGameAction& game);
};