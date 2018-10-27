#pragma once

namespace kh {

    namespace helper {

        template<typename ItemT, account_name ExportName>
        class singleton {

        public:
            singleton(const account_name code,
                      const account_name singleton_name)
                    : _code(code),
                      _singleton_name(singleton_name),
                      _table(code, singleton_name) {
            }

            typedef eosio::multi_index <ExportName, ItemT> table_t;

        protected:

            table_t _table;
            account_name _code;
            account_name _singleton_name;
        };

    }
}