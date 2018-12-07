#pragma once

namespace kh {
    namespace utils {

        template<typename TVal>
        void call(const account_name from, const account_name target, const account_name act_name, const TVal &&value) {
            eosio::action(eosio::permission_level{from, N(active)}, target, act_name, value).send();
        }

        template<typename TVal>
        void call_with_namestr(const account_name from, const account_name target, const char *act, const TVal &&value) {
            call(from, target, eosio::string_to_name(act), value);
        }
    }
}