#pragma once

#include "../utils/assert.hpp"
#include "./plugin.hpp"
#include "./context.hpp"

namespace kh {

    class plg_trans_code_only_be_eosio : plugin<ctx_transfer> {
    public:
        override void on_trigger(const ctx_transfer &ctx, const contract &contract) {
            kh::assert::equal(contract.code, N(eosio.token), "plg_trans_code_only_be_eosio : validate failed.")
            next(ctx, contract);
        };
    };

    class plg_transfer_send_transcal : plugin<ctx_transfer> {
    public:
        override void on_trigger(const ctx_transfer &ctx, const contract &contract) {
            auto code = contract.code;
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
                contract.on_transcal(from, to, quantity, func, args);
            }

            next(ctx, contract);
        };

    };

}