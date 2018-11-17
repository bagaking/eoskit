#pragma once

#include "./utils/assert.hpp"
#include "./helper/system_attr.hpp"

#include "./plugin/plugin.hpp"
#include "./plugin/context.hpp"
#include "./plugin/plg_transfer.hpp"


namespace kh {

    class contract : public eosio::contract {

    public: /** constructor */
        contract(
                const account_name self,
                const account_name code) //if code not equal to self, meas this call is triggered by other contracts.
                : eosio::contract(self),
                  _p_sa(nullptr),
                  _code(code) {
        };

    public: /** region for public event */

        void on_transfer(
                const account_name from,
                const account_name to,
                eosio::asset quantity,
                std::string memo
        ) {
            if(transfer_plugins != nullptr) {
                return;
            }
            auto ctx = ctx_transfer();
            ctx.from = from;
            ctx.to = to;
            ctx.quantity = quantity;
            ctx.memo = memo;

            transfer_plugins->trigger(ctx, *this);
        }
    protected: /** region for private event */

        virtual void on_transcal(const account_name from,
                                 const account_name to,
                                 eosio::asset quantity,
                                 std::string func,
                                 std::vector<std::string>& args) {
            if(transcal_plugins != nullptr) {
                return;
            }
            auto ctx = ctx_transcal();
            ctx.from = from;
            ctx.to = to;
            ctx.quantity = quantity;
            ctx.func = func;
            ctx.args = args;
            transcal_plugins->trigger(ctx, *this);
        }
    public: /** region for export */
        [[eosio::action]] void setattr(account_name key, const std::string val) {
            _set_my(key, val);
        }

    protected: /** fields */
        account_name _code;
        helper::system_attr* _p_sa;
        kh::plugin<kh::ctx_transfer>* transfer_plugins;
        kh::plugin<kh::ctx_transcal>* transcal_plugins;

    protected: /** region for utility */

        template <typename T>
        std::string _get_my(const uint64_t key, const T default_val) {
            if (_p_sa == nullptr) {
                _p_sa = new helper::system_attr(_self);
            }
            return _p_sa->ensure(key, default_val);
        }

        template <typename T>
        void _set_my(account_name key, const T val) {
            require_auth(_code);
            if (_p_sa == nullptr) {
                _p_sa = new helper::system_attr(_self);
            }
            _p_sa->set(key, val);
        }

        template<typename T>
        void _inline_action(const char *act, T &&value) {
            eosio::action(
                    eosio::permission_level{get_self(), N(active)},
                    get_self(),
                    eosio::string_to_name(act),
                    value)
                    .send();
        }

        void _transfer_token(const account_name to, const account_name token_code, eosio::asset token, std::string memo) {
            eosio::action(
                    eosio::permission_level{get_self(), N(active)},
                    token_code,
                    N(transfer),
                    make_tuple(get_self(), to, token, memo))
                    .send();
        }



    };
} // namespace kh

bool transfer_act(uint64_t action) {
    return action == N(transfer);
}

#define EXPORT_ABI(TYPE, MEMBERS) \
  extern "C" {  \
     \
     \
    void apply(uint64_t receiver, uint64_t code, uint64_t action) \
    { \
      eosio_assert(boost::is_base_of<kh::contract, TYPE>::value, "contract must extend the kh::contract"); \
      \
      if (action == N(onerror)) \
      { \
        eosio_assert(code == N(eosio), \
                     "onerror action's are only valid from the \"eosio\" system account"); \
      } \
      auto self = receiver; \
        \
      bool valid_internal_actions = code == self && !transfer_act(action); \
      bool valid_external_transfer = transfer_act(action) && code == N(eosio.token); \
      if (valid_internal_actions || valid_external_transfer || action == N(onerror)) \
      { \
        TYPE thiscontract(self, code); \
        switch (action) \
        { \
        case N(transfer): \
          execute_action(&thiscontract, &TYPE::on_transfer); \
          break; \
          EOSIO_API(TYPE, MEMBERS) \
        } \
        /* does not allow destructor of thiscontract to run: eosio_exit(0); */ \
      } \
    } \
  }
