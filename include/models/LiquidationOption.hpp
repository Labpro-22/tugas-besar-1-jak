#pragma once
#include <string>

class PropertyTile;

class LiquidationOption {
public:
    enum class Type { SELL_PROPERTY, MORTGAGE_PROPERTY };

private:
    Type type;
    PropertyTile* property;
    int amount;
    std::string description;

public:
    LiquidationOption(Type type, PropertyTile* property, int amount, const std::string& description) : type(type), property(property), amount(amount), description(description) {}
    Type getType() const { return type; }
    PropertyTile* getProperty() const { return property; }
    int getAmount() const { return amount; }
    std::string getDescription() const { return description; }
};