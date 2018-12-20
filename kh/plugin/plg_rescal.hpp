#pragma once

#include "./eoskit/kh/utils/assert.hpp"

#include "./eoskit/kh/plugin/plugin.hpp"
#include "./eoskit/kh/plugin/context.hpp"

#include "./eoskit/kh/contract.hpp"

#include "./plugin/plugin.hpp"
#include "./plugin/context.hpp"


namespace kh {


    class plg_rescal : public kh::plugin<std::string, kh::contract>  {
    public:
        void on_trigger(const std::string &ctx, kh::contract &contract_) override {
            if (ctx[0] != '@') {
                return;
            }
            auto start = ctx.begin();
            auto end = ctx.end();

            while (true) {
                auto start = std::find(start, end, '[');
                if (start == end) {
                    break;
                }
                auto tmp_end = end;
                auto end = std::find(start, end, ']');
                if (end == tmp_end) {
                    break;
                }
                auto col = std::find(start, end, ':');
                if (col == end) {
                    break;
                }
                auto func = ctx.substr(start-ctx.begin(), col-start);
                eosio::print(func);
                eosio::print("==================");
                std::vector<std::string> args;
                auto arg_start = col + 1;
                auto arg_end = end;
                while (true) {
                    auto dm = std::find(arg_start, arg_end, ',');
                    if (dm == arg_end) {
                        auto arg = ctx.substr(arg_start-ctx.begin(), arg_end-arg_start);
                        eosio::print("arg: ", arg);
                        args.push_back(arg);
                        break;
                    }
                    auto arg = ctx.substr(arg_start-ctx.begin(), dm-arg_start);
                    eosio::print("arg: ", arg);
                    args.push_back(arg);
                    arg_start = dm;
                    //contract_._inline_action(func.c_str(), std::make_tuple(args));
                }
            }


            /** go ahead */
            next(ctx, contract_);
        };
    };

}