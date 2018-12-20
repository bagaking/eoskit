#pragma once
namespace kh {

    namespace assert {

        const void ok(bool value, std::string reason) {
            eosio_assert(value, reason.c_str());
        }

        const void revert(std::string reason) {
            ok(false, reason);
        }

        template<typename T>
        const void equal(const T &a, const T &b, std::string assertion) {
            ok(a == b, assertion);
        }

        template<typename T>
        const void not_equal(const T &a, const T &b, std::string assertion) {
            ok(a != b, assertion);
        }

        const void code_must_be_eosio_token(account_name code) {
            equal(N(eosio.token), code, "invalid token publisher " + (eosio::name{code}).to_string());
        }

        const void is_valid_token(eosio::asset token) {
            auto sym = token.symbol;
            ok(sym.is_valid(), "invalid symbol name");
            ok(token.is_valid(), "Invalid token transfer");
            ok(token.amount > 0, "must bet a positive amount");
        }

        const void is_valid_token_of_symbol(eosio::asset token, eosio::symbol_type symbol) {
            is_valid_token(token);
            equal(token.symbol, symbol, "symbol of the token are not allowed");
        }

        const void is_system_account(account_name name) {
            ok(name == N(eosio.bpay) ||
               name == N(eosio.msig) ||
               name == N(eosio.names) ||
               name == N(eosio.ram) ||
               name == N(eosio.ramfee) ||
               name == N(eosio.saving) ||
               name == N(eosio.stake) ||
               name == N(eosio.token) ||
               name == N(eosio.vpay), "must be system account");
        }
    }
}