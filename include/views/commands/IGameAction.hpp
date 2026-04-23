#pragma once

#include <string>

// Kontrak aksi game yang dipanggil oleh layer Command.
class IGameAction {
public:
    virtual ~IGameAction() = default;

    virtual bool isGameActive() const = 0;

    virtual void lemparDadu() = 0;
    virtual void aturDadu(int x, int y) = 0;
    virtual void beliProperti() = 0;
    virtual void gadaiProperti(const std::string& tileCode) = 0;
    virtual void tebusProperti(const std::string& tileCode) = 0;
    virtual void bangunDiProperti(const std::string& tileCode) = 0;
    virtual void jualBangunan(const std::string& tileCode) = 0;
    virtual void tawar(int amount) = 0;
    virtual void lepasLelang() = 0;
    virtual void bayarDenda() = 0;

    virtual void gunakanKartuKemampuan(int cardIndex) = 0;
    virtual void gunakanKartuBebas() = 0;

    virtual void simpanGame(const std::string& filename) = 0;
    virtual void muatGame(const std::string& filename) = 0;
    virtual void cetakPapan() = 0;
    virtual void cetakAkta(const std::string& tileCode) = 0;
    virtual void cetakInventoriProperti() = 0;
    virtual void cetakLog(int limit) = 0;

    virtual void akhiriGiliran() = 0;
    virtual void bangkrut() = 0;
    virtual void quitGame() = 0;
};