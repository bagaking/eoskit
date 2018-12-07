#pragma once

#include "./utils/assert.hpp"
#include "./utils/util.hpp"

namespace kh {

    class contract_inline {
    public:
        contract_inline(const account_name self) : __self(self) {}

    public:

        template<typename TVal>
        void _inline_action(const char *action_str, const TVal &&value) {
            eosio::action(eosio::permission_level{__self, N(active)}, __self, eosio::string_to_name(action_str), value).send();
        }

        void _transfer_token(
                const account_name &to,
                const account_name &token_code_account,
                const eosio::asset &quantity,
                const std::string &memo) {
            utils::call(__self, token_code_account, N(transfer), make_tuple(__self, to, quantity, memo));
        }

        void _transfer_eos(
                const account_name &to,
                const eosio::asset &quantity,
                const std::string &memo) {
            utils::call(__self, N(eosio.token), N(transfer), make_tuple(__self, to, quantity, memo));
        }

    private:

        account_name __self;

    }; // namespace kh

}