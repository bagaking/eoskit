#pragma once

#include "../utils/assert.hpp"
#include "./plugin.hpp"
#include "./context.hpp"

namespace kh {

    class plg_trans_code_only_be_eosio : plugin<ctx_transfer> {
    public:
        override void on_trigger(const ctx_transfer &ctx) {
            kh::assert::equal(ctx.code, N(eosio.token), "plg_trans_code_only_be_eosio : validate failed.")
            next();
        };

    };

}