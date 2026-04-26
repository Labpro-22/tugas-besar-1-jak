#ifndef SKILLCARD_HPP
#define SKILLCARD_HPP

#include "Card.hpp"

class SkillCard : public Card {
public:
    SkillCard(std::string n, std::string d) : Card(n, d) {}
};

class MoveCard : public SkillCard {
private:
    int steps; // Jumlah langkah maju
public:
    MoveCard(int s);
    int getValue() const { return steps; }
};

class DiscountCard : public SkillCard {
private:
    int percent;  // Persentase diskon
    int duration; // Sisa durasi (1 giliran)
public:
    DiscountCard(int p, int dur);
    int getValue() const { return percent; }
    int getDuration() const { return duration; }
};

class ShieldCard : public SkillCard {
private:
    int duration; // Sisa durasi (1 giliran)
public:
    ShieldCard(int dur);
    int getDuration() const { return duration; }
};

class TeleportCard : public SkillCard {
public:
    TeleportCard();
};

class LassoCard : public SkillCard {
public:
    LassoCard();
};

class DemolitionCard : public SkillCard {
public:
    DemolitionCard();
};

#endif