#pragma once

namespace kh {

    class contract_base : public eosio::contract {
    public: /** constructor */
        contract_base(
                const account_name self,
                const account_name code) //if code not equal to self, meas this call is triggered by other contracts.
                : eosio::contract(self),
                  _code(code) {
        };

        account_name _get_code() {
            return _code;
        }

        account_name _get_self() {
            return _self;
        }

    protected: /** fields */
        account_name _code;
    };
} // namespace kh