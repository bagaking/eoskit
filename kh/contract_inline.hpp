#pragma once

#include "./utils/assert.hpp"
#include "./utils/util.hpp"

namespace kh {

    class contract_inline {
    public:
        contract_inline(const account_name self) : __self(self) {}

    public:
        template<typename T>
        void _inline_action(const char *act, const T &&value) {
            utils::call(__self, __self, act, value);
        }

        void _transfer_token(
                const account_name &to,
                const account_name &token_code_account,
                const eosio::asset &token,
                const std::string &memo) {
            utils::call(__self, token_code_account, act, make_tuple(__self, to, token, memo));
        }

    private:

        account_name __self;

    }; // namespace kh

}