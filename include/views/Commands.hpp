#pragma once
#include "Command.hpp"
#include "../../include/core/IGameAction.hpp"
#include <string>
#include <vector>
#include <stdexcept>

// ===== RollDiceCommand — LEMPAR_DADU =====
class RollDiceCommand : public Command {
    public:
    void execute(IGameAction& game) override {
        game.rollDice();
    }
    string getName() const override { 
        return "LEMPAR_DADU"; 
    }
};

// ===== SetDiceCommand — ATUR_DADU X Y =====
class SetDiceCommand : public Command {
    private:
    int x_, y_;

    public:
    SetDiceCommand(int x, int y) : x_(x), y_(y) {}

    void execute(IGameAction& game) override {
        game.setDice(x_, y_);
    }
    string getName() const override { 
        return "ATUR_DADU"; 
    }
};

// ===== BuyCommand — BELI =====
class BuyCommand : public Command {
    public:
    void execute(IGameAction& game) override {
        game.buyCurrentProperty();
    }
    string getName() const override { 
        return "BELI"; 
    }
};

// ===== MortgageCommand — GADAI <kode_petak> [<kode_petak> ...] =====
class MortgageCommand : public Command {
    private:
    vector<string> tileCodes_;
    public:
    explicit MortgageCommand(vector<string> tileCodes) : tileCodes_(move(tileCodes)) {}

    void execute(IGameAction& game) override {
        for (const auto& code : tileCodes_) {
            game.mortgageProperty(code);
        }
    }
    string getName() const override { 
        return "GADAI"; 
    }
};

// ===== RedeemCommand — TEBUS <kode_petak> =====
class RedeemCommand : public Command {
    private:
    string tileCode_;
    
    public:
    explicit RedeemCommand(string tileCode) : tileCode_(move(tileCode)) {}

    void execute(IGameAction& game) override {
        game.redeemProperty(tileCode_);
    }
    string getName() const override { 
        return "TEBUS"; 
    }
};

// ===== BuildCommand — BANGUN <kode_petak> =====
class BuildCommand : public Command {
    private:
    string tileCode_;
    
    public:
    explicit BuildCommand(string tileCode) : tileCode_(move(tileCode)) {}

    void execute(IGameAction& game) override {
        game.buildOnProperty(tileCode_);
    }
    string getName() const override { 
        return "BANGUN"; 
    }
};

// ===== SellBuildingCommand — JUAL_BANGUNAN <kode_petak> =====
class SellBuildingCommand : public Command {
    private:
    string tileCode_;
    
    public:
    explicit SellBuildingCommand(string tileCode) : tileCode_(move(tileCode)) {}

    void execute(IGameAction& game) override {
        game.sellBuildingOnProperty(tileCode_);
    }
    string getName() const override { 
        return "JUAL_BANGUNAN"; 
    }
};

// ===== BidCommand — TAWAR <jumlah> =====
class BidCommand : public Command {
    private:
    int amount_;
    
    public:
    explicit BidCommand(int amount) : amount_(amount) {}

    void execute(IGameAction& game) override {
        game.placeBid(amount_);
    }
    string getName() const override { 
        return "TAWAR"; 
    }
};

// ===== PassAuctionCommand — LEPAS =====
class PassAuctionCommand : public Command {
    public:
    void execute(IGameAction& game) override {
        game.passAuction();
    }
    string getName() const override { 
        return "LEPAS"; 
    }
};

// ===== UseSkillCommand — GUNAKAN_KEMAMPUAN <indeks_kartu> =====
class UseSkillCommand : public Command {
    private:
    int cardIndex_;
    
    public:
    explicit UseSkillCommand(int cardIndex) : cardIndex_(cardIndex) {}

    void execute(IGameAction& game) override {
        game.useSkillCard(cardIndex_);
    }
    string getName() const override { 
        return "GUNAKAN_KEMAMPUAN"; 
    }
};

// ===== PayJailFineCommand — BAYAR_DENDA =====
class PayJailFineCommand : public Command {
    public:
    void execute(IGameAction& game) override {
        game.payJailFine();
    }
    string getName() const override { 
        return "BAYAR_DENDA"; 
    }
};

// ===== UseJailFreeCardCommand — GUNAKAN_KARTU_BEBAS =====
class UseJailFreeCardCommand : public Command {
    public:
    void execute(IGameAction& game) override {
        game.useJailFreeCard();
    }
    string getName() const override { 
        return "GUNAKAN_KARTU_BEBAS"; 
    }
};

// ===== SaveCommand — SIMPAN <filename> =====
class SaveCommand : public Command {
    private:
    string filename_;
    
    public:
    explicit SaveCommand(string filename) : filename_(move(filename)) {}

    void execute(IGameAction& game) override {
        game.saveGame(filename_);
    }
    string getName() const override { 
        return "SIMPAN"; 
    }
};

// ===== LoadCommand — MUAT <filename> =====
class LoadCommand : public Command {
    private:
    string filename_;
    
    public:
    explicit LoadCommand(string filename) : filename_(move(filename)) {}

    void execute(IGameAction& game) override {
        game.loadGame(filename_);
    }
    string getName() const override { 
        return "MUAT"; 
    }
};

// ===== PrintBoardCommand — CETAK_PAPAN =====
class PrintBoardCommand : public Command {
    public:
    void execute(IGameAction& game) override {
        game.printBoard();
    }
    string getName() const override { 
        return "CETAK_PAPAN"; 
    }
};

// ===== PrintDeedCommand — CETAK_AKTA [<kode_petak>] =====
class PrintDeedCommand : public Command {
    private:
    string tileCode_;

    public:
    explicit PrintDeedCommand(string tileCode = "") : tileCode_(move(tileCode)) {}

    void execute(IGameAction& game) override {
        game.printDeed(tileCode_);
    }
    string getName() const override { 
        return "CETAK_AKTA"; 
    }
};

// ===== PrintPropertyCommand — CETAK_PROPERTI =====
class PrintPropertyCommand : public Command {
    public:
    void execute(IGameAction& game) override {
        game.printPropertyInventory();
    }
    string getName() const override { 
        return "CETAK_PROPERTI"; 
    }
};

// ===== PrintLogCommand — CETAK_LOG [<limit>] =====
class PrintLogCommand : public Command {
    private:
    int limit_;
    
    public:
    explicit PrintLogCommand(int limit = -1) : limit_(limit) {}

    void execute(IGameAction& game) override {
        game.printLog(limit_);
    }
    string getName() const override { 
        return "CETAK_LOG"; 
    }
};