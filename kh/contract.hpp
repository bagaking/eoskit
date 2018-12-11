#pragma once

#include "./contract_base.hpp"
#include "./contract_attr.hpp"
#include "./contract_inline.hpp"
#include "./contract_res.hpp"
#include "./contract_itm.hpp"
#include "./contract_awd.hpp"

#include "./plugin/plugin.hpp"
#include "./plugin/context.hpp"

#include "./helper/system_attr.hpp"
#include "./utils/assert.hpp"

namespace kh {

    class contract
            : public contract_base,
              public contract_attr,
              public contract_inline,
              public contract_res,
              public contract_itm,
              public contract_awd {

    public:
        typedef plugin<kh::ctx_transfer, kh::contract> plg_transfer_t;
        typedef plugin<kh::ctx_transcal, kh::contract> plg_transcal_t;

    public: /** constructor */
        contract(
                const account_name self,
                const account_name code) //if code not equal to self, meas this call is triggered by other contracts.
                : contract_base(self, code),
                  contract_attr(self),
                  contract_inline(self),
                  contract_res(self),
                  contract_itm(self),
                  contract_awd(self) {
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

    protected: /** fields */
        kh::plugin<kh::ctx_transfer, kh::contract> *transfer_plugins;
        kh::plugin<kh::ctx_transcal, kh::contract> *transcal_plugins;

    };
} // namespace kh


#define EXPORT_ABI(TYPE, MEMBERS) \
extern "C" {  \
    void apply(uint64_t receiver, uint64_t code, uint64_t action) { \
        eosio::print("export :", receiver, ",", code, ",", action," ||| ");\
        eosio_assert(boost::is_base_of<kh::contract, TYPE>::value, "contract must extend the kh::contract"); \
        eosio_assert(action != N(onerror) || code == N(eosio), "onerror action's are only valid from the 'eosio' system account"); \
        auto self = receiver; \
        TYPE thiscontract(self, code); \
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

#define KH_EXPORT(TYPE, API_NAMES) EXPORT_ABI(TYPE, API_NAMES KH_EXPORT_ATTR KH_EXPORT_RES KH_EXPORT_ITM KH_EXPORT_AWD)