#pragma once

#include "../utils/assert.hpp"
#include "./plugin.hpp"
#include "./context.hpp"

namespace kh {

    class plg_transcal_checking_whitelist : plugin<kh::ctx_transcal, kh::contract> {
    public:
        override void on_trigger(const ctx_transcal &ctx, const kh::contract &cont) {
            kh::assert::equal(ctx.func, "deposit", "plg_trans_code_only_be_eosio : only deposit are accepted.");
            /** go ahead */
            next(ctx, cont);
        };

    };


    class plg_transcal_checking_pause : plugin<kh::ctx_transcal, kh::contract> {
    public:
        override void on_trigger(const ctx_transcal &ctx, const kh::contract &cont) {
            kh::assert::not_equal(cont._get_my(N(pause), "false"), std::string("true"), "game paused");
            /** go ahead */
            next(ctx, cont);
        };

    };

}