#pragma once
#include "Command.hpp"
#include "utils/NimonspoliException.hpp"
#include "AkhiriGiliran.hpp"
#include "AturDadu.hpp"
#include "Bangkrut.hpp"
#include "Bangun.hpp"
#include "Bantuan.hpp"
#include "BayarDenda.hpp"
#include "Beli.hpp"
#include "CetakAkta.hpp"
#include "CetakLog.hpp"
#include "CetakPapan.hpp"
#include "CetakProperti.hpp"
#include "CetakStatus.hpp"
#include "Gadai.hpp"
#include "GunakanKartuBebas.hpp"
#include "GunakanKemampuan.hpp"
#include "JualBangunan.hpp"
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

    // Pastikan minimal N argumen terpenuhi
    static void requireMinArgCount(const std::vector<std::string>& tokens, int minCount, const std::string& cmd) {
        int got = static_cast<int>(tokens.size()) - 1;
        if (got < minCount) {
            throw InvalidInputException(cmd + ": butuh minimal " + std::to_string(minCount) + " argumen, tapi dapat " + std::to_string(got) + ".");
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

        // ===== LEMPAR_DADU =====
        if (cmd == "LEMPAR_DADU") {
            requireArgCount(tokens, 0, cmd);
            return new LemparDaduCommand();
        }

        // ===== ATUR_DADU X Y =====
        if (cmd == "ATUR_DADU") {
            requireArgCount(tokens, 2, cmd);
            int x = parseIntArg(tokens[1], cmd);
            int y = parseIntArg(tokens[2], cmd);
            if (x < 1 || x > 6 || y < 1 || y > 6) {
                throw InvalidInputException("ATUR_DADU: nilai dadu harus antara 1-6.");
            }
            return new AturDaduCommand(x, y);
        }

        // ===== BELI =====
        if (cmd == "BELI") {
            requireArgCount(tokens, 0, cmd);
            return new BeliCommand();
        }

        // ===== GADAI <kode> [<kode> ...] =====
        if (cmd == "GADAI") {
            requireMinArgCount(tokens, 1, cmd);
            std::vector<std::string> codes(tokens.begin() + 1, tokens.end());
            return new GadaiCommand(std::move(codes));
        }

        // ===== TEBUS <kode> =====
        if (cmd == "TEBUS") {
            requireArgCount(tokens, 1, cmd);
            return new TebusCommand(tokens[1]);
        }

        // ===== BANGUN <kode> =====
        if (cmd == "BANGUN") {
            requireArgCount(tokens, 1, cmd);
            return new BangunCommand(tokens[1]);
        }

        // ===== JUAL_BANGUNAN <kode> =====
        if (cmd == "JUAL_BANGUNAN") {
            requireArgCount(tokens, 1, cmd);
            return new JualBangunanCommand(tokens[1]);
        }

        // ===== TAWAR <jumlah> =====
        if (cmd == "TAWAR") {
            requireArgCount(tokens, 1, cmd);
            int amount = parseIntArg(tokens[1], cmd);
            if (amount <= 0) {
                throw InvalidInputException("TAWAR: jumlah tawaran harus lebih dari 0.");
            }
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
            if (idx < 0) {
                throw InvalidInputException("GUNAKAN_KEMAMPUAN: indeks kartu tidak valid.");
            }
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
            return new SimpanCommand(tokens[1]);
        }

        // ===== MUAT <filename> =====
        if (cmd == "MUAT") {
            requireArgCount(tokens, 1, cmd);
            return new MuatCommand(tokens[1]);
        }

        // ===== CETAK_PAPAN =====
        if (cmd == "CETAK_PAPAN") {
            requireArgCount(tokens, 0, cmd);
            return new CetakPapanCommand();
        }

        // ===== CETAK_AKTA [<kode>] =====
        if (cmd == "CETAK_AKTA") {
            std::string code = "";
            if (tokens.size() > 1) {
                code = tokens[1];
            }
            return new CetakAktaCommand(code);
        }

        // ===== CETAK_PROPERTI =====
        if (cmd == "CETAK_PROPERTI") {
            requireArgCount(tokens, 0, cmd);
            return new CetakPropertiCommand();
        }

        // ===== CETAK_LOG [<limit>] =====
        if (cmd == "CETAK_LOG") {
            int limit = -1;
            if (tokens.size() > 1) {
                limit = parseIntArg(tokens[1], cmd);
                if (limit <= 0) {
                    throw InvalidInputException("CETAK_LOG: limit harus bilangan positif.");
                }
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

        // ===== BANGKRUT =====
        if (cmd == "BANGKRUT") {
            requireArgCount(tokens, 0, cmd);
            return new BangkrutCommand();
        }

        // ===== BANTUAN =====
        if (cmd == "BANTUAN") {
            requireArgCount(tokens, 0, cmd);
            return new BantuanCommand();
        }

        // ===== CETAK_STATUS =====
        if (cmd == "CETAK_STATUS") {
            requireArgCount(tokens, 0, cmd);
            return new CetakStatusommand();
        }

        // Tidak dikenali
        throw InvalidInputException("Perintah tidak dikenal: \"" + cmd + "\".");
    }
};