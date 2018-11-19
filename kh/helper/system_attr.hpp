#pragma once

#include "base.hpp"
#include "table_type.hpp"

#include "../utils/assert.hpp"

namespace kh {

    namespace helper {

        class system_attr : singleton<field_all_in_one_t, N(attributes)> { //sa

        public:
            system_attr(account_name code) :
                    singleton(code, code) {
            }

            bool exist(const uint64_t key) {
                auto itr = tb().find(key);
                return itr != tb().end();
            }

            auto get_itr(const uint64_t key) {
                auto itr = tb().find(key);
                kh::assert::not_equal(itr, tb().end(), "key not exist");
                return itr;
            }

            std::string get_str(const uint64_t key) {
                return get_itr(key)->vstr;
            }

            uint32_t get_int(const uint64_t key) {
                return get_itr(key)->vint;
            }

            account_name get_aname(const uint64_t key) {
                return get_itr(key)->vaname;
            }

            eosio::asset get_asset(const uint64_t key) {
                return get_itr(key)->vasset;
            }


            std::string ensure(const uint64_t key, const std::string default_val = "_"){
                auto itr = tb().find(key);
                if (itr == tb().end()) {
                    tb().emplace(_code, [&](auto &row) {
                        row.key = key;
                        row.vstr = default_val;
                    });
                    return default_val;
                } else{
                    return itr->vstr;
                }
            }

            uint32_t ensure(const uint64_t key, const uint32_t default_val = 0){
                auto itr = tb().find(key);
                if (itr == tb().end()) {
                    tb().emplace(_code, [&](auto &row) {
                        row.key = key;
                        row.vint = default_val;
                    });
                    return default_val;
                } else{
                    return itr->vint;
                }
            }

            account_name ensure(const uint64_t key, const account_name default_val = 0){
                auto itr = tb().find(key);
                if (itr == tb().end()) {
                    tb().emplace(_code, [&](auto &row) {
                        row.key = key;
                        row.vaname = default_val;
                    });
                    return default_val;
                } else{
                    return itr->vaname;
                }
            }

            eosio::asset ensure(const uint64_t key, const eosio::asset default_val){
                auto itr = tb().find(key);
                if (itr == tb().end()) {
                    tb().emplace(_code, [&](auto &row) {
                        row.key = key;
                        row.vasset = default_val;
                    });
                    return default_val;
                } else{
                    return itr->vasset;
                }
            }

            void set(account_name key, const std::string val) {
                require_auth(_code);

                auto itr = tb().find(key);

                if (itr == tb().end()) {
                    tb().emplace(_code, [&](auto &row) {
                        row.key = key;
                        row.vstr = val;
                    });
                } else {
                    tb().modify(itr, _code, [&](auto &row) {
                        row.vstr = val;
                    });
                }
            }

            void set(account_name key, const uint32_t val) {
                require_auth(_code);

                auto itr = tb().find(key);

                if (itr == tb().end()) {
                    tb().emplace(_code, [&](auto &row) {
                        row.key = key;
                        row.vint = val;
                    });
                } else {
                    tb().modify(itr, _code, [&](auto &row) {
                        row.vint = val;
                    });
                }
            }

            void set(account_name key, const account_name val) {
                require_auth(_code);

                auto itr = tb().find(key);

                if (itr == tb().end()) {
                    tb().emplace(_code, [&](auto &row) {
                        row.key = key;
                        row.vaname = val;
                    });
                } else {
                    tb().modify(itr, _code, [&](auto &row) {
                        row.vaname = val;
                    });
                }
            }

            void set(account_name key, const eosio::asset val) {
                require_auth(_code);

                auto itr = tb().find(key);

                if (itr == tb().end()) {
                    tb().emplace(_code, [&](auto &row) {
                        row.key = key;
                        row.vasset = val;
                    });
                } else {
                    tb().modify(itr, _code, [&](auto &row) {
                        row.vasset = val;
                    });
                }
            }

        };
    }
}