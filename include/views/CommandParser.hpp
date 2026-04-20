#pragma once
#include "Command.hpp"
#include "Commands.hpp"
#include "../../include/utils/NimonspoliException.hpp"

#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <algorithm>
#include <cctype>

// Terima string mentah dari user, lalu kembalikan unique_ptr<Command> yang siap di-execute()
class CommandParser {
    private:
    // Bikin input jadi token berdasarkan whitespace
    static vector<string> tokenize(const string& input) {
        vector<string> tokens;
        istringstream iss(input);
        string token;
        while (iss >> token) {
            // Jadiin uppercase semua biar case insensitive
            transform(token.begin(), token.end(), token.begin(), ::toupper);
            tokens.push_back(token);
        }
        return tokens;
    }

    // Validasi jumlah argumen (gak termasuk nama command itu sendiri)
    static void requireArgCount(const vector<string>& tokens, int expected, const string& cmd) {
        int got = static_cast<int>(tokens.size()) - 1;
        if (got != expected) {
            throw InvalidInputException(cmd + ": butuh " + to_string(expected) + " argumen, tapi dapat " + to_string(got) + ".");
        }
    }

    // Pastikan minimal N argumen
    static void requireMinArgCount(const vector<string>& tokens, int minCount, const string& cmd) {
        int got = static_cast<int>(tokens.size()) - 1;
        if (got < minCount) {
            throw InvalidInputException(cmd + ": butuh minimal " + to_string(minCount) + " argumen, tapi dapat " + to_string(got) + ".");
        }
    }

    // Parse token ke int, lempar InvalidInputException kalo bukan angka
    static int parseIntArg(const string& token, const string& cmd) {
        try {
            size_t pos;
            int val = stoi(token, &pos);
            if (pos != token.size()) throw invalid_argument("bukan int murni");
            return val;
        } catch (...) {
            throw InvalidInputException(cmd + ": argumen \"" + token + "\" harus berupa angka bulat.");
        }
    }

    public:
    // Parse satu baris input menjadi unique_ptr<Command>
    static unique_ptr<Command> parse(const string& rawInput) {
        auto tokens = tokenize(rawInput);
        if (tokens.empty()) {
            throw InvalidInputException("Input kosong.");
        }

        const string& cmd = tokens[0];

        // ===== LEMPAR_DADU =====
        if (cmd == "LEMPAR_DADU") {
            requireArgCount(tokens, 0, cmd);
            return make_unique<RollDiceCommand>();
        }

        // ===== ATUR_DADU X Y =====
        if (cmd == "ATUR_DADU") {
            requireArgCount(tokens, 2, cmd);
            int x = parseIntArg(tokens[1], cmd);
            int y = parseIntArg(tokens[2], cmd);
            if (x < 1 || x > 6 || y < 1 || y > 6) {
                throw InvalidInputException("ATUR_DADU: nilai dadu harus antara 1-6.");
            }
            return make_unique<SetDiceCommand>(x, y);
        }

        // ===== BELI =====
        if (cmd == "BELI") {
            requireArgCount(tokens, 0, cmd);
            return make_unique<BuyCommand>();
        }

        // ===== GADAI <kode> [<kode> ...] =====
        if (cmd == "GADAI") {
            requireMinArgCount(tokens, 1, cmd);
            vector<string> codes(tokens.begin() + 1, tokens.end());
            return make_unique<MortgageCommand>(move(codes));
        }

        // ===== TEBUS <kode> =====
        if (cmd == "TEBUS") {
            requireArgCount(tokens, 1, cmd);
            return make_unique<RedeemCommand>(tokens[1]);
        }

        // ===== BANGUN <kode> =====
        if (cmd == "BANGUN") {
            requireArgCount(tokens, 1, cmd);
            return make_unique<BuildCommand>(tokens[1]);
        }

        // ===== JUAL_BANGUNAN <kode> =====
        if (cmd == "JUAL_BANGUNAN") {
            requireArgCount(tokens, 1, cmd);
            return make_unique<SellBuildingCommand>(tokens[1]);
        }

        // ===== TAWAR <jumlah> =====
        if (cmd == "TAWAR") {
            requireArgCount(tokens, 1, cmd);
            int amount = parseIntArg(tokens[1], cmd);
            if (amount <= 0) {
                throw InvalidInputException("TAWAR: jumlah tawaran harus lebih dari 0.");
            }
            return make_unique<BidCommand>(amount);
        }

        // ===== LEPAS =====
        if (cmd == "LEPAS") {
            requireArgCount(tokens, 0, cmd);
            return make_unique<PassAuctionCommand>();
        }

        // ===== GUNAKAN_KEMAMPUAN <indeks> =====
        if (cmd == "GUNAKAN_KEMAMPUAN") {
            requireArgCount(tokens, 1, cmd);
            int idx = parseIntArg(tokens[1], cmd);
            if (idx < 0) {
                throw InvalidInputException("GUNAKAN_KEMAMPUAN: indeks kartu tidak valid.");
            }
            return make_unique<UseSkillCommand>(idx);
        }

        // ===== BAYAR_DENDA =====
        if (cmd == "BAYAR_DENDA") {
            requireArgCount(tokens, 0, cmd);
            return make_unique<PayJailFineCommand>();
        }

        // ===== GUNAKAN_KARTU_BEBAS =====
        if (cmd == "GUNAKAN_KARTU_BEBAS") {
            requireArgCount(tokens, 0, cmd);
            return make_unique<UseJailFreeCardCommand>();
        }

        // ===== SIMPAN <filename> =====
        if (cmd == "SIMPAN") {
            requireArgCount(tokens, 1, cmd);
            return make_unique<SaveCommand>(tokens[1]);
        }

        // ===== MUAT <filename> =====
        if (cmd == "MUAT") {
            requireArgCount(tokens, 1, cmd);
            return make_unique<LoadCommand>(tokens[1]);
        }

        // ===== CETAK_PAPAN =====
        if (cmd == "CETAK_PAPAN") {
            requireArgCount(tokens, 0, cmd);
            return make_unique<PrintBoardCommand>();
        }

        // ===== CETAK_AKTA [<kode>] =====
        if (cmd == "CETAK_AKTA") {
            string code = (tokens.size() > 1) ? tokens[1] : "";
            return make_unique<PrintDeedCommand>(code);
        }

        // ===== CETAK_PROPERTI =====
        if (cmd == "CETAK_PROPERTI") {
            requireArgCount(tokens, 0, cmd);
            return make_unique<PrintPropertyCommand>();
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
            return make_unique<PrintLogCommand>(limit);
        }

        // Tidak dikenali
        throw InvalidInputException("Perintah tidak dikenal: \"" + cmd + "\".");
    }
};