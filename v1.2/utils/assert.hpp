#pragma once
namespace kh {

    namespace assert {

        const void ok(bool value, std::string reason){
            eosio_assert(value, reason.c_str());
        }

        const void revert(std::string reason){
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
            equal(N(eosio.token), code, "invalid token publisher");
        }

        const void is_valid_token_of_symbol(eosio::asset token, eosio::symbol_type symbol) {
            ok(token.is_valid(), "Invalid token transfer");
            ok(token.amount > 0, "must bet a positive amount");
            equal(token.symbol, symbol, "Only core token allowed");
        }
    }
}