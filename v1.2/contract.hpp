#pragma once

namespace kh {

    class contract : public eosio::contract {
    public:
        contract(
                const account_name self,
                const account_name code)
                : eosio::contract(self),
                  _code(code) {};

        void transfer(
                const account_name from,
                const account_name to,
                eosio::asset quantity,
                std::string memo
        ) {
            if (memo.length() > 4 && memo[0] == '@' && memo[1] == '[' && memo[2] != ']') { //min str : @[a]
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
                on_transcal(from, to, quantity, func, args);
            } else {
                on_transfer(from, to, quantity, memo);
            }
        }

    protected:
        account_name _code;

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

        virtual void on_transfer(const account_name from,
                                 const account_name to,
                                 eosio::asset token,
                                 std::string memo) = 0;

        virtual void on_transcal(const account_name from,
                                 const account_name to,
                                 eosio::asset token,
                                 std::string func,
                                 std::vector <std::string> args) = 0;
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
          execute_action(&thiscontract, &TYPE::transfer); \
          break; \
          EOSIO_API(TYPE, MEMBERS) \
        } \
        /* does not allow destructor of thiscontract to run: eosio_exit(0); */ \
      } \
    } \
  }
