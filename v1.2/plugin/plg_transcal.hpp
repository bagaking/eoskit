#pragma once

#include "./plugin.hpp"

namespace kh {


    class plg_transcal_check_pause : public plugin<kh::ctx_transcal, kh::contract> {
    public:
        void on_trigger(const ctx_transcal &ctx, kh::contract &contract_) override {
            kh::assert::not_equal(contract_._get_my(N(pause), "false"), std::string("true"), "game paused");
            /** go ahead */
            next(ctx, contract_);
        };
    };

    class plg_transcal_check_in_whitelist : public plugin<kh::ctx_transcal, kh::contract> {
    public:
        void on_trigger(const ctx_transcal &ctx, kh::contract &contract_) override {
            kh::assert::ok(ctx.func == "deposit", "plg_trans_code_only_be_eosio : only deposit are accepted.");
            /** go ahead */
            next(ctx, contract_);
        };
    };

    class plg_transcal_check_auth_of_from : public plugin<kh::ctx_transcal, kh::contract> {
    public:
        void on_trigger(const ctx_transcal &ctx, kh::contract &contract_) override {
            require_auth(ctx.from);

            /** go ahead */
            next(ctx, contract_);
        };
    };

}