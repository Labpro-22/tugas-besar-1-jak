#ifndef SKILLCARD_HPP
#define SKILLCARD_HPP

#include "Card.hpp"

class SkillCard : public Card {
public:
    SkillCard(std::string n, std::string d) : Card(n, d) {}
    
    // Mengaktifkan efek kartu secara manual
    virtual void use(Player& player, Game& game) = 0; 
};

class MoveCard : public SkillCard {
private:
    int steps; // Jumlah langkah maju
public:
    MoveCard(int s);
    int getValue() const { return steps; }
    void use(Player& player, Game& game) override;
};

class DiscountCard : public SkillCard {
private:
    int percent;  // Persentase diskon
    int duration; // Sisa durasi (1 giliran)
public:
    DiscountCard(int p, int dur);
    int getValue() const { return percent; }
    int getDuration() const { return duration; }
    void use(Player& player, Game& game) override;
};

class ShieldCard : public SkillCard {
private:
    int duration; // Sisa durasi (1 giliran)
public:
    ShieldCard(int dur);
    int getDuration() const { return duration; }
    void use(Player& player, Game& game) override;
};

class TeleportCard : public SkillCard {
public:
    TeleportCard();
    void use(Player& player, Game& game) override;
};

class LassoCard : public SkillCard {
public:
    LassoCard();
    void use(Player& player, Game& game) override;
};

class DemolitionCard : public SkillCard {
public:
    DemolitionCard();
    void use(Player& player, Game& game) override;
};

#endif