#include "models/SpecialTile.hpp"

// SpecialTile

SpecialTile::SpecialTile(int idx, std::string cd, std::string nm)
    : Tile(idx, cd, nm) {}

TileType SpecialTile::getTileType() const { return TileType::SPECIAL; }

// GoTile

GoTile::GoTile(int idx, std::string cd, std::string nm, int slr)
    : SpecialTile(idx, cd, nm), salary(slr) {}

void GoTile::onLanded(Player& player, Game& game) {
    game.addLog(player.getUsername(), "GERAK", "Mendarat di petak GO.");
    player += salary;
    game.getCLIRenderer()->printInfo(
        player.getUsername() + " mendarat di petak GO, mendapat gaji " +
        game.getCLIRenderer()->formatMoney(salary) + "."
    );
    game.addLog(player.getUsername(), "GO", "Mendapat gaji " + game.getCLIRenderer()->formatMoney(salary) + ".");
}

void GoTile::onPassed(Player& player, Game& game) {
    player += salary;
    game.getCLIRenderer()->printInfo(
        player.getUsername() + " melewati petak GO, mendapat gaji " +
        game.getCLIRenderer()->formatMoney(salary) + "."
    );
    game.addLog(
        player.getUsername(), "GO", "Melewati petak GO, mendapat gaji " + 
        game.getCLIRenderer()->formatMoney(salary) + "."
    );
}

JailTile::JailTile(int idx, std::string cd, std::string nm, int fn)
    : SpecialTile(idx, cd, nm), fine(fn) {}

void JailTile::onLanded(Player& player, Game& game) {
    game.addLog(player.getUsername(), "GERAK", "Mendarat di petak Penjara.");
    std::string message;
    if (player.isJailed()) {
        message = player.getUsername() + " sedang berada di penjara.";
    } else {
        message = player.getUsername() + " hanya mampir di penjara.";
    }
    game.getCLIRenderer()->printInfo(message);
}

// GoToJailTile

GoToJailTile::GoToJailTile(int idx, std::string cd, std::string nm)
    : SpecialTile(idx, cd, nm) {}

void GoToJailTile::onLanded(Player& player, Game& game) {
    game.addLog(player.getUsername(), "GERAK", "Mendarat di petak Pergi ke Penjara.");
    game.getCLIRenderer()->printInfo(
        player.getUsername() + " mendarat di petak Pergi ke Penjara."
    );
    if (player.isShieldActive()) {
        game.getCLIRenderer()->printInfo(
            "Efek perlindungan aktif, " + player.getUsername() + " terlindungi dari masuk penjara."
        );
    } else {
        game.getCLIRenderer()->printInfo(
            player.getUsername() + " langsung masuk penjara."
        );
        game.sendPlayerToJail(player, "Mendarat di petak Pergi ke Penjara");
    }
}

// FreeParkingTile

FreeParkingTile::FreeParkingTile(int idx, std::string cd, std::string nm)
    : SpecialTile(idx, cd, nm) {}

void FreeParkingTile::onLanded(Player& player, Game& game) {
    game.addLog(player.getUsername(), "GERAK", "Mendarat di petak Bebas Parkir.");
    game.getCLIRenderer()->printInfo(
        player.getUsername() + " mendarat di petak Bebas Parkir, tidak melakukan apa-apa."
    );
}
