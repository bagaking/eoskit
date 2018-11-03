#pragma once

#include "base.hpp"
#include "table_type.hpp"

namespace kh {

    namespace helper {

        class attribute : singleton<single_val_item_string_t, N(attributes)> {

        public:
            attribute(account_name code) :
                    singleton(code) {
            }

            bool exist(const uint64_t key) {
                auto itr = tb().find(key);
                return itr != tb().end();
            }

            std::string get(const uint64_t key) {
                auto itr = tb().find(key);
                kh::assert::not_equal(itr, tb().end(), "key not exist");
                return itr->val;
            }

            void set(account_name key, const std::string val) {
                require_auth(_code);

                auto itr = tb().find(key);

                if (itr == tb().end()) {
                    tb().emplace(_code, [&](auto &row) {
                        row.key = key;
                        row.val = val;
                    });
                } else {
                    tb().modify(itr, _code, [&](auto &row) {
                        row.val = val;
                    });
                }
            }

        };
    }
}