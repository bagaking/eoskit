#pragma once

#include "./helper/system_attr.hpp"
#include "./utils/assert.hpp"

namespace kh {

    class contract_inline {
    public:
        contract_inline(const account_name self) : __self(self) {}

    public:
        template<typename T>
        void _inline_action(const char *act, const T &&value) {
            eosio::action(
                    eosio::permission_level{__self, N(active)},
                    __self,
                    eosio::string_to_name(act),
                    value)
                    .send();
        }

        void _transfer_token(
                const account_name &to,
                const account_name &token_code,
                const eosio::asset &token,
                const std::string &memo) {
            eosio::action(
                    eosio::permission_level{__self, N(active)},
                    token_code,
                    N(transfer),
                    make_tuple(__self, to, token, memo))
                    .send();
        }

    private:

        account_name __self;

    }; // namespace kh

}