#pragma once

#include "./contract_base.hpp"
#include "./contract_attr.hpp"

#include "./plugin/plugin.hpp"
#include "./plugin/context.hpp"

#include "./helper/system_attr.hpp"
#include "./utils/assert.hpp"

namespace kh {

    class contract : public contract_base, public contract_attr {
    public:
        typedef plugin<kh::ctx_transfer, kh::contract> plg_transfer_t;
        typedef plugin<kh::ctx_transcal, kh::contract> plg_transcal_t;

    public: /** constructor */
        contract(
                const account_name self,
                const account_name code) //if code not equal to self, meas this call is triggered by other contracts.
                : contract_base(self, code),
                  contract_attr(self) {
        };

        /** region for public event */
        //    public:
        void on_transfer(
                account_name &from,
                account_name &to,
                eosio::asset &quantity,
                std::string &memo
        ) {
            if (transfer_plugins == nullptr) { return; }
            ctx_transfer ctx = {.from = from, .to = to, .quantity = quantity, .memo = memo};
            transfer_plugins->trigger(ctx, *this);
        }

        /** region for private event */
        //    protected:
        void on_transcal(account_name &from,
                         account_name &to,
                         eosio::asset &quantity,
                         std::string &func,
                         std::vector <std::string> &args) {
            if (transcal_plugins == nullptr) {
                return;
            }
            ctx_transcal ctx = {.from = from, .to = to, .quantity = quantity, .func = func, .args = args};
            transcal_plugins->trigger(ctx, *this);
        }

        template<typename T>
        void _inline_action(const char *act, const T &&value) {
            eosio::action(
                    eosio::permission_level{get_self(), N(active)},
                    get_self(),
                    eosio::string_to_name(act),
                    value)
                    .send();
        }

        void _transfer_token(
                const account_name &to,
                const account_name &token_code,
                const eosio::asset &token,
                const std::string &memo) {
            eosio::action(
                    eosio::permission_level{get_self(), N(active)},
                    token_code,
                    N(transfer),
                    make_tuple(get_self(), to, token, memo))
                    .send();
        }

    protected: /** fields */
        kh::plugin<kh::ctx_transfer, kh::contract> *transfer_plugins;
        kh::plugin<kh::ctx_transcal, kh::contract> *transcal_plugins;

    };
} // namespace kh


#define EXPORT_ABI(TYPE, MEMBERS) \
extern "C" {  \
    void apply(uint64_t receiver, uint64_t code, uint64_t action) { \
        eosio_assert(boost::is_base_of<kh::contract, TYPE>::value, "contract must extend the kh::contract"); \
        eosio_assert(action != N(onerror) || code == N(eosio), "onerror action's are only valid from the 'eosio' system account"); \
        auto self = receiver; \
        TYPE thiscontract(self, code); \
        eosio::print("export :", self, ",", code, " ||| ");\
        eosio::print("try call action :", action, " ||| ");\
        switch (action) { \
            case N(transfer): \
                if(code == self) break; /* cannot receive transfer triggered by self */ \
                /* to verify that code is eosio.token, using plugin plg_transfer_validate_eos_token in the contract */ \
                execute_action(&thiscontract, &TYPE::on_transfer); \
                break; \
            EOSIO_API(TYPE, MEMBERS) \
            default: \
                eosio::print("call action error", action);\
                break;\
        } \
    } \
}

#define KH_EXPORT(TYPE, MEMBERS) EXPORT_ABI(TYPE, MEMBERS(setattr))