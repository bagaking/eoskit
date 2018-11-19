#pragma once

#include "./plugin.hpp"

namespace kh {


    class plg_transcal_check_pause : public plugin<kh::ctx_transcal, kh::contract> {
    public:
        void on_trigger(const ctx_transcal &ctx, kh::contract &contract_) override {
            eosio::print("plg_transcal_check_pause");
            kh::assert::not_equal(contract_._get_my(N(pause), "false"), std::string("true"), "plg_transcal_check_pause : game paused");
            /** go ahead */
            next(ctx, contract_);
        };
    };

    class plg_transcal_check_in_whitelist : public plugin<kh::ctx_transcal, kh::contract> {
    public:
        void on_trigger(const ctx_transcal &ctx, kh::contract &contract_) override {
            eosio::print("plg_transcal_check_in_whitelist");
            kh::assert::ok(ctx.func == "deposit", "plg_transcal_check_in_whitelist : only deposit are accepted.");
            /** go ahead */
            next(ctx, contract_);
        };
    };

    class plg_transcal_check_auth_of_from : public plugin<kh::ctx_transcal, kh::contract> {
    public:
        void on_trigger(const ctx_transcal &ctx, kh::contract &contract_) override {
            eosio::print("plg_transcal_check_auth_of_from : auth error");
            require_auth(ctx.from);

            /** go ahead */
            next(ctx, contract_);
        };
    };

}