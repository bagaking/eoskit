#pragma once

#include "./plugin/plugin.hpp"
#include "./plugin/context.hpp"

#include "./helper/system_attr.hpp"
#include "./utils/assert.hpp"

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

        /** region for public event */
        //    public:
        void on_transfer(
                const account_name &from,
                const account_name &to,
                const eosio::asset &quantity,
                const std::string &memo
        );

        /** region for private event */
        //    protected:
        void on_transcal(const account_name &from,
                         const account_name &to,
                         const eosio::asset &quantity,
                         const std::string &func,
                         const std::vector <std::string> &args);
        /** region for export */
        //    public:
        [[eosio::action]] void setattr(account_name key, const std::string val);

        /** region for utility */
        //    protected:

        account_name _get_code() {
            return _code;
        }

        template<typename T>
        void _set_my(account_name key, const T val);

        template<typename T>
        std::string _get_my(const uint64_t key, const T default_val);

        template<typename T>
        void _inline_action(const char *act, T &&value);

        void
        _transfer_token(const account_name to, const account_name token_code, eosio::asset token, std::string memo);

    protected: /** fields */
        account_name _code;
        helper::system_attr *_p_sa;
        kh::plugin<kh::ctx_transfer, kh::contract> *transfer_plugins;
        kh::plugin<kh::ctx_transcal, kh::contract> *transcal_plugins;

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
