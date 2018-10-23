#pragma once

namespace kh {
    namespace helper {

        class counter {

        public:

            counter(account_name contract, account_name ctr_name) : _contract(contract), _ctr_name(ctr_name) {
                ctr_table counters(_contract, _contract);
                auto iterator = counters.find(_ctr_name);
                if (iterator == counters.end()) {
                    counters.emplace(_contract, [&](auto &row) {
                        row.ctr_name = _ctr_name;
                        row.count = 10;
                    });
                }
            }

            int get() {
                ctr_table counters(_contract, _contract);
                auto iterator = counters.find(_ctr_name);
                return iterator->count;
            }

            int inc(account_name caller) {
                ctr_table counters(_contract, _contract);
                auto iterator = counters.find(_ctr_name);
                auto rc = 0;
                counters.modify(iterator, caller, [&](auto &row) {
                    row.count += 1;
                    rc = row.count;
                });
                return rc;
            }

        private :
            account_name _contract;
            account_name _ctr_name;


            //@abi table maker i64
            struct [[eosio::table]] ctr {
                account_name ctr_name;
                uint32_t count;

                uint64_t primary_key() const {
                    return ctr_name;
                }

                EOSLIB_SERIALIZE(ctr, (ctr_name)(count)
                )
            };

            typedef eosio::multi_index<"counters"_n, ctr> ctr_table;
        };

    }
}