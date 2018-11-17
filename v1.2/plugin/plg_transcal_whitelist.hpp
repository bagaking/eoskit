#pragma once

#include "../utils/assert.hpp"
#include "./plugin.hpp"
#include "./context.hpp"

namespace kh {

    class plg_trans_code_only_be_eosio : plugin<ctx_transcal> {
    public:
        override void on_trigger(const ctx_transcal &ctx) {
            kh::assert::equal(ctx.func, "deposit", "plg_trans_code_only_be_eosio : only deposit are accepted.")
            next();
        };

    };

}