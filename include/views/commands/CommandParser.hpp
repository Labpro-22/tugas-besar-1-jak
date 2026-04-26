#pragma once
#include "Command.hpp"
#include "utils/NimonspoliException.hpp"
#include "AkhiriGiliran.hpp"
#include "AturDadu.hpp"
#include "Bangun.hpp"
#include "Bantuan.hpp"
#include "BayarDenda.hpp"
#include "CetakAkta.hpp"
#include "CetakLog.hpp"
#include "CetakPapan.hpp"
#include "CetakProperti.hpp"
#include "CetakStatus.hpp"
#include "Gadai.hpp"
#include "GunakanKartuBebas.hpp"
#include "GunakanKemampuan.hpp"
#include "LemparDadu.hpp"
#include "Lepas.hpp"
#include "Muat.hpp"
#include "Quit.hpp"
#include "Simpan.hpp"
#include "Tawar.hpp"
#include "Tebus.hpp"

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>

// Terima string mentah dari user, lalu kembalikan objek Command yang siap di-execute()
class CommandParser {
    private:
    // Bikin input jadi token berdasarkan whitespace
    static std::vector<std::string> tokenize(const std::string& input) {
        std::vector<std::string> tokens;
        std::istringstream iss(input);
        std::string token;
        while (iss >> token) {
            // Jadiin uppercase semua biar case insensitive
            std::transform(token.begin(), token.end(), token.begin(), ::toupper);
            tokens.push_back(token);
        }
        return tokens;
    }

    // Validasi jumlah argumen
    static void requireArgCount(const std::vector<std::string>& tokens, int expected, const std::string& cmd) {
        int got = static_cast<int>(tokens.size()) - 1;
        if (got != expected) {
            throw InvalidInputException(cmd + ": butuh " + std::to_string(expected) + " argumen, tapi dapat " + std::to_string(got) + ".");
        }
    }

    // Parse token ke int, lempar InvalidInputException kalo bukan angka
    static int parseIntArg(const std::string& token, const std::string& cmd) {
        try {
            std::size_t pos;
            int val = std::stoi(token, &pos);
            if (pos != token.size()) {
                throw std::invalid_argument("Bukan berupa angka/integer!");
            }
            return val;
        } catch (...) {
            throw InvalidInputException(cmd + ": argumen \"" + token + "\" harus berupa angka bulat.");
        }
    }

    public:
    // Parse satu baris input menjadi objek Command
    static Command* parse(const std::string& rawInput) {
        auto tokens = tokenize(rawInput);
        if (tokens.empty()) {
            throw InvalidInputException("Input kosong.");
        }

        const std::string& cmd = tokens[0];

        // Command yang dipanggil manual
        // ===== LEMPAR_DADU =====
        if (cmd == "LEMPAR_DADU") {
            requireArgCount(tokens, 0, cmd);
            return new LemparDaduCommand();
        }

        // ===== ATUR_DADU <x> <y> =====
        if (cmd == "ATUR_DADU") {
            requireArgCount(tokens, 2, cmd);
            int x = parseIntArg(tokens[1], cmd);
            int y = parseIntArg(tokens[2], cmd);
            return new AturDaduCommand(x, y);
        }

        // ===== GADAI =====
        if (cmd == "GADAI") {
            requireArgCount(tokens, 0, cmd);
            return new GadaiCommand();
        }

        // ===== TEBUS <kode> =====
        if (cmd == "TEBUS") {
            requireArgCount(tokens, 0, cmd);
            return new TebusCommand();
        }

        // ===== BANGUN <kode> =====
        if (cmd == "BANGUN") {
            requireArgCount(tokens, 1, cmd);
            return new BangunCommand(tokens[1]);
        }

        // ===== TAWAR <jumlah> =====
        if (cmd == "TAWAR") {
            requireArgCount(tokens, 1, cmd);
            int amount = parseIntArg(tokens[1], cmd);
            return new TawarCommand(amount);
        }

        // ===== LEPAS =====
        if (cmd == "LEPAS") {
            requireArgCount(tokens, 0, cmd);
            return new LepasCommand();
        }

        // ===== GUNAKAN_KEMAMPUAN <indeks> =====
        if (cmd == "GUNAKAN_KEMAMPUAN") {
            requireArgCount(tokens, 1, cmd);
            int idx = parseIntArg(tokens[1], cmd);
            return new GunakanKemampuanCommand(idx);
        }

        // ===== BAYAR_DENDA =====
        if (cmd == "BAYAR_DENDA") {
            requireArgCount(tokens, 0, cmd);
            return new BayarDendaCommand();
        }

        // ===== GUNAKAN_KARTU_BEBAS =====
        if (cmd == "GUNAKAN_KARTU_BEBAS") {
            requireArgCount(tokens, 0, cmd);
            return new GunakanKartuBebasCommand();
        }

        // ===== SIMPAN <filename> =====
        if (cmd == "SIMPAN") {
            requireArgCount(tokens, 1, cmd);
            // Ambil filename dari rawInput langsung biar ga kena toupper
            size_t spacePos = rawInput.find(' ');
            std::string filename = (spacePos != std::string::npos) ? rawInput.substr(spacePos + 1) : "";
            return new SimpanCommand(filename);
        }

        // ===== MUAT <filename> =====
        if (cmd == "MUAT") {
            throw InvalidInputException("MUAT hanya bisa dilakukan sebelum permainan dimulai.");
        }

        // ===== CETAK_PAPAN =====
        if (cmd == "CETAK_PAPAN") {
            requireArgCount(tokens, 0, cmd);
            return new CetakPapanCommand();
        }

        // ===== CETAK_AKTA <kode> =====
        if (cmd == "CETAK_AKTA") {
            requireArgCount(tokens, 0, cmd);
            return new CetakAktaCommand();
        }

        // ===== CETAK_PROPERTI =====
        if (cmd == "CETAK_PROPERTI") {
            requireArgCount(tokens, 0, cmd);
            return new CetakPropertiCommand();
        }

        // ===== CETAK_LOG <limit> =====
        if (cmd == "CETAK_LOG") {
            int limit = -1;
            if (tokens.size() > 1) {
                limit = parseIntArg(tokens[1], cmd);
            }
            return new CetakLogCommand(limit);
        }

        // ===== QUIT =====
        if (cmd == "QUIT") {
            requireArgCount(tokens, 0, cmd);
            return new QuitCommand();
        }

        // ===== AKHIRI_GILIRAN =====
        if (cmd == "AKHIRI_GILIRAN") {
            requireArgCount(tokens, 0, cmd);
            return new AkhiriGiliranCommand();
        }

        // ===== BANTUAN =====
        if (cmd == "BANTUAN") {
            requireArgCount(tokens, 0, cmd);
            return new BantuanCommand();
        }

        // ===== CETAK_STATUS =====
        if (cmd == "CETAK_STATUS") {
            requireArgCount(tokens, 0, cmd);
            return new CetakStatusCommand();
        }
        
        // Tidak dikenali
        throw InvalidInputException("Perintah tidak dikenal: \"" + cmd + "\".");
    }
};