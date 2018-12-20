#pragma once

#include "./plugin.hpp"

namespace kh {

    class plg_transfer_validate_eos_token : public plugin<kh::ctx_transfer, kh::contract> {
    public:

        void on_trigger(const ctx_transfer &ctx, kh::contract &contract_) override {
            eosio::print("plg_transfer_validate_eos_token");

            kh::assert::code_must_be_eosio_token(contract_._get_code());
            kh::assert::is_valid_token_of_symbol(ctx.quantity, S(4, EOS));
            /** go ahead */
            next(ctx, contract_);
        }

    };

    class plg_transfer_send_transcal : public plugin<kh::ctx_transfer, kh::contract> {
    public:

        void on_trigger(const ctx_transfer &ctx, kh::contract &contract_) override {
            if(contract_._get_code() == N(eosio.token)) return next(ctx, contract_); // code must be eosio.token
            if(ctx.from == contract_._get_self()) return next(ctx, contract_); // if it is a receipt send by _self, jump over
            if(ctx.to != contract_._get_self()) return next(ctx, contract_); // if the receiver is not _self, jump over
            if(ctx.quantity.symbol != S(4, EOS)) return next(ctx, contract_);

            kh::assert::is_valid_token_of_symbol(ctx.quantity, S(4, EOS)); // must be EOS token

            eosio::print("plg_transfer_send_transcal");
            auto code = contract_._get_code();
            auto from = ctx.from;
            auto to = ctx.to;
            auto quantity = ctx.quantity;
            auto memo = ctx.memo;

            auto call_type = memo[0];
            auto correct_call_type = (call_type == '@' || call_type == '#');
            if (memo.length() > 4 && correct_call_type && memo[1] == '[' && memo[2] != ']') { //min str : @[a]
                auto pos_col = memo.find(':');
                auto pos_end = memo.find(']');
                std::string func;
                std::vector <std::string> args;
                if (pos_col != std::string::npos) {
                    func = memo.substr(2, pos_col - 2);
                    auto pos = pos_col + 1;
                    auto pos_prev = pos;
                    while (true) {
                        if (pos = memo.find(',', pos), pos >= pos_end || pos == std::string::npos) {
                            args.push_back(memo.substr(pos_prev, pos_end - pos_prev));
                            break;
                        }
                        args.push_back(memo.substr(pos_prev, pos - pos_prev));
                        pos_prev = ++pos;
                    }
                } else {
                    func = memo.substr(2, pos_end - 2);
                }
                contract_.on_transcal(from, to, quantity, func, args);
            }

            /** go ahead */
            next(ctx, contract_);
        };

    };


}