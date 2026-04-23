#ifndef ACTIONCARD_HPP
#define ACTIONCARD_HPP

#include "Card.hpp"

enum class CardType { CHANCE, COMMUNITY_CHEST }; // CHANCE = Kesempatan, COMMUNITY_CHEST = Dana Umum

class ActionCard : public Card {
protected:
    CardType cardType;
public:
    ActionCard(std::string n, std::string d, CardType t) : Card(n, d), cardType(t) {}
    
    // Mengeksekusi instruksi instan kepada pemain
    virtual void execute(Player& player, Game& game) = 0; 
    
    CardType getCardType() const { return cardType; }
};

// --- Subkelas Kesempatan ---

class GoToNearestStationCard : public ActionCard {
public:
    GoToNearestStationCard();
    void execute(Player& player, Game& game) override;
};

class MoveBackCard : public ActionCard {
public:
    MoveBackCard();
    void execute(Player& player, Game& game) override;
};

class GoToJailCard : public ActionCard {
public:
    GoToJailCard();
    void execute(Player& player, Game& game) override;
};

// --- Subkelas Dana Umum ---

class BirthdayCard : public ActionCard {
private:
    int amount; // M100
public:
    BirthdayCard();
    void execute(Player& player, Game& game) override;
};

class DoctorFeeCard : public ActionCard {
private:
    int fee; // M700
public:
    DoctorFeeCard();
    void execute(Player& player, Game& game) override;
};

class NyalegCard : public ActionCard {
private:
    int amount; // M200
public:
    NyalegCard();
    void execute(Player& player, Game& game) override;
};

#endif