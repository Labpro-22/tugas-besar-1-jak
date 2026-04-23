#include <iostream>
#include <cassert>
#include "models/ActionCard.hpp"
#include "models/SkillCard.hpp"
#include "models/CardDeck.hpp"

// =================================================================
// MOCK CLASSES (Hanya untuk memenuhi parameter fungsi execute & use)
// =================================================================
class Player {};
class Game {};

// =================================================================
// FUNGSI TESTING
// =================================================================

void testActionCards() {
    std::cout << "--- Menguji ActionCard (Kesempatan & Dana Umum) ---\n";
    
    GoToNearestStationCard c1;
    MoveBackCard c2;
    GoToJailCard c3;
    BirthdayCard c4;
    DoctorFeeCard c5;
    NyalegCard c6;

    // Memastikan tipe deck sudah benar sesuai spesifikasi
    assert(c1.getCardType() == CardType::CHANCE);
    assert(c4.getCardType() == CardType::COMMUNITY_CHEST);

    std::cout << "[OK] " << c1.getName() << " | " << c1.getDescription() << "\n";
    std::cout << "[OK] " << c2.getName() << " | " << c2.getDescription() << "\n";
    std::cout << "[OK] " << c3.getName() << " | " << c3.getDescription() << "\n";
    std::cout << "[OK] " << c4.getName() << " | " << c4.getDescription() << "\n";
    std::cout << "[OK] " << c5.getName() << " | " << c5.getDescription() << "\n";
    std::cout << "[OK] " << c6.getName() << " | " << c6.getDescription() << "\n";
    std::cout << "Semua ActionCard berhasil diinstansiasi!\n\n";
}

void testSkillCards() {
    std::cout << "--- Menguji SkillCard (Kemampuan Spesial) ---\n";
    
    MoveCard s1(5);
    DiscountCard s2(30, 1);
    ShieldCard s3(1);
    TeleportCard s4;
    LassoCard s5;
    DemolitionCard s6;

    std::cout << "[OK] " << s1.getName() << " | " << s1.getDescription() << "\n";
    std::cout << "[OK] " << s2.getName() << " | " << s2.getDescription() << "\n";
    std::cout << "[OK] " << s3.getName() << " | " << s3.getDescription() << "\n";
    std::cout << "[OK] " << s4.getName() << " | " << s4.getDescription() << "\n";
    std::cout << "[OK] " << s5.getName() << " | " << s5.getDescription() << "\n";
    std::cout << "[OK] " << s6.getName() << " | " << s6.getDescription() << "\n";
    std::cout << "Semua SkillCard berhasil diinstansiasi!\n\n";
}

void testCardDeckMechanics() {
    std::cout << "--- Menguji Mekanisme CardDeck<T> ---\n";
    
    CardDeck<SkillCard> deck;
    
    // 1. Menambahkan kartu ke deck
    deck.addCardToDeck(new MoveCard(4));
    deck.addCardToDeck(new ShieldCard(1));
    deck.addCardToDeck(new LassoCard());
    
    std::cout << "3 Kartu ditambahkan ke dalam deck.\n";

    // 2. Draw kartu
    SkillCard* drawn1 = deck.draw();
    std::cout << "Draw 1: " << drawn1->getName() << "\n";
    assert(drawn1->getName() == "MoveCard");

    SkillCard* drawn2 = deck.draw();
    std::cout << "Draw 2: " << drawn2->getName() << "\n";
    
    SkillCard* drawn3 = deck.draw();
    std::cout << "Draw 3: " << drawn3->getName() << "\n";

    // 3. Draw saat kartu habis (sebelum ada yang di-discard)
    SkillCard* emptyDraw = deck.draw();
    if (emptyDraw == nullptr) {
        std::cout << "Draw 4: nullptr (Deck kosong, discard pile kosong)\n";
    }
    assert(emptyDraw == nullptr);

    // 4. Discard kartu yang sudah dipakai
    std::cout << "\nMembuang kartu-kartu yang telah dipakai ke Discard Pile...\n";
    deck.discard(drawn1);
    deck.discard(drawn2);
    deck.discard(drawn3);

    // 5. Test Reshuffle (Menarik kartu saat deck kosong tapi discard pile ada isinya)
    std::cout << "Mencoba Draw lagi. Deck seharusnya melakukan kocok ulang (reshuffle)...\n";
    SkillCard* reshuffledCard = deck.draw();
    
    assert(reshuffledCard != nullptr);
    std::cout << "[OK] Reshuffle berhasil! Kartu yang didapat: " << reshuffledCard->getName() << "\n";
    
    // Kembalikan ke discard untuk dicleanup oleh destructor
    deck.discard(reshuffledCard); 
    
    std::cout << "Mekanisme CardDeck<T> berjalan sempurna sesuai spesifikasi!\n\n";
}

