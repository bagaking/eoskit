#pragma once
namespace kh {

    namespace assert {
        const void code_must_be_eosio_token(account_name code) {
            eosio_assert(N(eosio.token) == code, "invalid token publisher");
        }

        const void is_valid_token_of_symbol(eosio::asset token, eosio::symbol_type symbol) {
            eosio_assert(token.is_valid(), "Invalid token transfer");
            eosio_assert(token.symbol == symbol, "Only core token allowed");
            eosio_assert(token.amount > 0, "must bet a positive amount");
        }

        template<typename T>
        const void equal(const T &a, const T &b, std::string assertion) {
            eosio_assert(a == b, assertion.c_str());
        }

        template<typename T>
        const void not_equal(const T &a, const T &b, std::string assertion) {
            eosio_assert(a != b, assertion.c_str());
        }
    }
}