#pragma once

#include "base.hpp"
#include "table_type.hpp"

namespace kh {

    namespace helper {

        /**
         * counter to create
         *
         * @warning
         * If you want to update this class, be careful to be compatible with old data.
         * otherwise it may cause irreparable errors in other logic.
         */
        class counter : singleton<single_val_item_uint32_t, N(counters)> {

        protected:
            account_name _counter_name;

        public:
            counter(account_name code,
                    account_name counter_name) :
                    singleton(code, code),
                    _counter_name(counter_name) {
                auto itr = tb().find(_counter_name);
                if (itr == tb().end()) {
                    tb().emplace(_code, [&](auto &row) {
                        row.key = _counter_name;
                        row.val = 1;
                    });
                }
            }

            /**
             * get current value of the counter
             * @return {uint32_t}
             */
            uint32_t get() {
                auto itr = tb().find(_counter_name);
                kh::assert::not_equal(itr, tb().end(), "counter not exist");
                return itr->val;
            }

            /**
             * inc with a delta value
             * @param {account_name} auth - can be inc by user's permission
             * @param {int} delta - can only be a positive number, default is 1
             * @return the new count after the inc operation
             */
            uint32_t inc(account_name auth, uint32_t delta = 1) {
                auto itr = tb().find(_counter_name);
                uint32_t new_count = 0;
                tb().modify(itr, auth, [&](auto &row) {
                    row.val += delta;
                    new_count = row.val;
                });
                return new_count;
            }

            /**
             * set a value directly
             * @param val
             *
             * @warning
             *      require **_code** authority.
             *      it is recommended to use the **set** interface **only** when data is incorrect.
             */
            void set(uint32_t val) {
                require_auth(_code);
                auto itr = tb().find(_counter_name);
            }

        };
    }
}