#pragma once
#include "Command.hpp"
#include "../../include/core/IGameAction.hpp"
#include <string>
#include <vector>
#include <stdexcept>

// ===== RollDiceCommand — LEMPAR_DADU =====
class RollDiceCommand : public Command
{
public:
    void execute(IGameAction &game) override
    {
        game.rollDice();
    }
    std::string getName() const override
    {
        return "LEMPAR_DADU";
    }
};

// ===== SetDiceCommand — ATUR_DADU X Y =====
class SetDiceCommand : public Command
{
private:
    int x, y;

public:
    SetDiceCommand(int x, int y) : x(x), y(y) {}

    void execute(IGameAction &game) override
    {
        game.setDice(x, y);
    }
    std::string getName() const override
    {
        return "ATUR_DADU";
    }
};

// ===== BuyCommand — BELI =====
class BuyCommand : public Command
{
public:
    void execute(IGameAction &game) override
    {
        game.buyCurrentProperty();
    }
    std::string getName() const override
    {
        return "BELI";
    }
};

// ===== MortgageCommand — GADAI <kode_petak> [<kode_petak> ...] =====
class MortgageCommand : public Command
{
private:
    std::vector<string> tileCodes;

public:
    explicit MortgageCommand(std::vector<string> tileCodes) : tileCodes(std::move(tileCodes)) {}

    void execute(IGameAction &game) override
    {
        for (const auto &code : tileCodes)
        {
            game.mortgageProperty(code);
        }
    }
    std::string getName() const override
    {
        return "GADAI";
    }
};

// ===== RedeemCommand — TEBUS <kode_petak> =====
class RedeemCommand : public Command
{
private:
    std::string tileCode;

public:
    explicit RedeemCommand(std::string tileCode) : tileCode(std::move(tileCode)) {}

    void execute(IGameAction &game) override
    {
        game.redeemProperty(tileCode);
    }
    std::string getName() const override
    {
        return "TEBUS";
    }
};

// ===== BuildCommand — BANGUN <kode_petak> =====
class BuildCommand : public Command
{
private:
    std::string tileCode;

public:
    explicit BuildCommand(std::string tileCode) : tileCode(std::move(tileCode)) {}

    void execute(IGameAction &game) override
    {
        game.buildOnProperty(tileCode);
    }
    std::string getName() const override
    {
        return "BANGUN";
    }
};

// ===== SellBuildingCommand — JUAL_BANGUNAN <kode_petak> =====
class SellBuildingCommand : public Command
{
private:
    std::string tileCode;

public:
    explicit SellBuildingCommand(std::string tileCode) : tileCode(std::move(tileCode)) {}

    void execute(IGameAction &game) override
    {
        game.sellBuildingOnProperty(tileCode);
    }
    std::string getName() const override
    {
        return "JUAL_BANGUNAN";
    }
};

// ===== BidCommand — TAWAR <jumlah> =====
class BidCommand : public Command
{
private:
    int amount;

public:
    explicit BidCommand(int amount) : amount(amount) {}

    void execute(IGameAction &game) override
    {
        game.placeBid(amount);
    }
    std::string getName() const override
    {
        return "TAWAR";
    }
};

// ===== PassAuctionCommand — LEPAS =====
class PassAuctionCommand : public Command
{
public:
    void execute(IGameAction &game) override
    {
        game.passAuction();
    }
    std::string getName() const override
    {
        return "LEPAS";
    }
};

// ===== UseSkillCommand — GUNAKAN_KEMAMPUAN <indeks_kartu> =====
class UseSkillCommand : public Command
{
private:
    int cardIndex;

public:
    explicit UseSkillCommand(int cardIndex) : cardIndex(cardIndex) {}

    void execute(IGameAction &game) override
    {
        game.useSkillCard(cardIndex);
    }
    std::string getName() const override
    {
        return "GUNAKAN_KEMAMPUAN";
    }
};

// ===== PayJailFineCommand — BAYAR_DENDA =====
class PayJailFineCommand : public Command
{
public:
    void execute(IGameAction &game) override
    {
        game.payJailFine();
    }
    std::string getName() const override
    {
        return "BAYAR_DENDA";
    }
};

// ===== UseJailFreeCardCommand — GUNAKAN_KARTU_BEBAS =====
class UseJailFreeCardCommand : public Command
{
public:
    void execute(IGameAction &game) override
    {
        game.useJailFreeCard();
    }
    std::string getName() const override
    {
        return "GUNAKAN_KARTU_BEBAS";
    }
};

// ===== SaveCommand — SIMPAN <filename> =====
class SaveCommand : public Command
{
private:
    std::string filename;

public:
    explicit SaveCommand(std::string filename) : filename(std::move(filename)) {}

    void execute(IGameAction &game) override
    {
        game.saveGame(filename);
    }
    std::string getName() const override
    {
        return "SIMPAN";
    }
};

// ===== LoadCommand — MUAT <filename> =====
class LoadCommand : public Command
{
private:
    std::string filename;

public:
    explicit LoadCommand(std::string filename) : filename(std::move(filename)) {}

    void execute(IGameAction &game) override
    {
        game.loadGame(filename);
    }
    std::string getName() const override
    {
        return "MUAT";
    }
};

// ===== PrintBoardCommand — CETAK_PAPAN =====
class PrintBoardCommand : public Command
{
public:
    void execute(IGameAction &game) override
    {
        game.printBoard();
    }
    std::string getName() const override
    {
        return "CETAK_PAPAN";
    }
};

// ===== PrintDeedCommand — CETAK_AKTA [<kode_petak>] =====
class PrintDeedCommand : public Command
{
private:
    std::string tileCode;

public:
    explicit PrintDeedCommand(std::string tileCode = "") : tileCode(std::move(tileCode)) {}

    void execute(IGameAction &game) override
    {
        game.printDeed(tileCode);
    }
    std::string getName() const override
    {
        return "CETAK_AKTA";
    }
};

// ===== PrintPropertyCommand — CETAK_PROPERTI =====
class PrintPropertyCommand : public Command
{
public:
    void execute(IGameAction &game) override
    {
        game.printPropertyInventory();
    }
    std::string getName() const override
    {
        return "CETAK_PROPERTI";
    }
};

// ===== PrintLogCommand — CETAK_LOG [<limit>] =====
class PrintLogCommand : public Command
{
private:
    int limit;

public:
    explicit PrintLogCommand(int limit = -1) : limit(limit) {}

    void execute(IGameAction &game) override
    {
        game.printLog(limit);
    }
    std::string getName() const override
    {
        return "CETAK_LOG";
    }
};