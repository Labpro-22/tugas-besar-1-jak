#include <cassert>
#include <iostream>
#include <string>

#include "views/commands/CommandParser.hpp"
#include "views/commands/IGameAction.hpp"

void testActionCards();
void testSkillCards();
void testCardDeckMechanics();

class MockGameAction : public IGameAction {
public:
	bool active = true;
	int lastSkillCardIndex = -1;
	bool usedFreeCard = false;
	bool endTurnCalled = false;
	bool bankruptCalled = false;
	std::string lastTileCode;
	std::string lastFilename;
	int lastLogLimit = -1;

	bool isGameActive() const override { return active; }

	void lemparDadu() override {}
	void aturDadu(int x, int y) override {
		(void)x;
		(void)y;
	}
	void beliProperti() override {}
	void gadaiProperti(const std::string& tileCode) override { lastTileCode = tileCode; }
	void tebusProperti(const std::string& tileCode) override { lastTileCode = tileCode; }
	void bangunDiProperti(const std::string& tileCode) override { lastTileCode = tileCode; }
	void jualBangunan(const std::string& tileCode) override { lastTileCode = tileCode; }
	void tawar(int amount) override { (void)amount; }
	void lepasLelang() override {}
	void bayarDenda() override {}

	void gunakanKartuKemampuan(int cardIndex) override { lastSkillCardIndex = cardIndex; }
	void gunakanKartuBebas() override { usedFreeCard = true; }

	void simpanGame(const std::string& filename) override { lastFilename = filename; }
	void muatGame(const std::string& filename) override { lastFilename = filename; }
	void cetakPapan() override {}
	void cetakAkta(const std::string& tileCode) override { lastTileCode = tileCode; }
	void cetakInventoriProperti() override {}
	void cetakLog(int limit) override { lastLogLimit = limit; }

	void akhiriGiliran() override { endTurnCalled = true; }
	void bangkrut() override { bankruptCalled = true; }
	void quitGame() override { active = false; }
};

static bool executeAndDelete(Command* cmd, IGameAction& game) {
	assert(cmd != nullptr);
	const bool turnEnded = cmd->execute(game);
	delete cmd;
	return turnEnded;
}

static void testCardCommandIntegration() {
	std::cout << "--- Menguji Integrasi Command untuk Kartu ---\n";

	MockGameAction game;

	bool turnEnded = executeAndDelete(CommandParser::parse("GUNAKAN_KEMAMPUAN 2"), game);
	assert(!turnEnded);
	assert(game.lastSkillCardIndex == 2);

	turnEnded = executeAndDelete(CommandParser::parse("GUNAKAN_KARTU_BEBAS"), game);
	assert(!turnEnded);
	assert(game.usedFreeCard);

	turnEnded = executeAndDelete(CommandParser::parse("AKHIRI_GILIRAN"), game);
	assert(turnEnded);
	assert(game.endTurnCalled);

	turnEnded = executeAndDelete(CommandParser::parse("QUIT"), game);
	assert(turnEnded);
	assert(!game.isGameActive());

	bool invalidThrown = false;
	try {
		Command* invalid = CommandParser::parse("GUNAKAN_KEMAMPUAN ABC");
		delete invalid;
	} catch (const InvalidInputException&) {
		invalidThrown = true;
	}
	assert(invalidThrown);

	std::cout << "Integrasi command kartu berjalan sesuai harapan!\n\n";
}

int main() {
	std::cout << "========================================\n";
	std::cout << "MEMULAI PENGUJIAN KELAS KARTU NIMONSPOLI\n";
	std::cout << "========================================\n\n";

	testActionCards();
	testSkillCards();
	testCardDeckMechanics();
	testCardCommandIntegration();

	std::cout << "========================================\n";
	std::cout << "SEMUA PENGUJIAN SELESAI TANPA ERROR (ASSERTION PASSED)\n";
	std::cout << "========================================\n";

	return 0;
}
