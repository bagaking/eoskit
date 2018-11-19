#pragma once

namespace kh {

    namespace helper {

        template<typename ItemT, account_name ExportName>
        class singleton {

        public:
            singleton(const account_name &code,
                      const account_name &singleton_key)
                    : _code(code),
                      _singleton_key(singleton_key),
                      _table(code, singleton_key) {
            }

            typedef eosio::multi_index <ExportName, ItemT> table_t;

            table_t &tb(){
                return _table;
            }

        protected:
            account_name _code;
            account_name _singleton_key;

        private:
            table_t _table;
        };

    }
}