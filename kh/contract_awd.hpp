#pragma once

#include "./utils/assert.hpp"
#include "./utils/util.hpp"

#include "./helper/table_type.hpp"

namespace kh {

    //@abi table maker i64
    struct [[eosio::table]] award_t {
        uint64_t key;
        eosio::asset left;

        uint64_t primary_key() const { return key; }

        EOSLIB_SERIALIZE(res_t, (balance)
        )
    };

    class contract_awd {
    public:
        contract_awd(const account_name self) : __self(self) {}

        [[eosio::action]] void awdreceipt(account_name user, uint64_t award_id, eosio::asset award) {
            require_auth(__self);
            require_recipient(user);
        }

        inline eosio::asset _get_awards_left(uint64_t eosio::symbol_name sym) const;

    private:
        account_name __self;

    private:
        typedef eosio::multi_index<N(awd.info), helper::res_t> awd_table_t;

        /**
         * append awards
         * @attention award's symbol, which are defined by the first award, will be verified
         * @param award_id - id of the awards
         * @param award - quantity
         */
        void _awd_append(uint64_t award_id, eosio::asset award);

        /**
         * record when someone takes the award
         * @attention award's amount will be verified in this function (must less-equal than the left of the award_id)
         * @param user - who takes the award
         * @param award_id
         * @param award - quantity
         */
        void _awd_take(account_name user, uint64_t award_id, eosio::asset award);
    }; // namespace kh

    eosio::asset contract_awd::_get_awards_left(uint64_t eosio, ::symbol_name sym) const {
        awd_table_t awards(__self, __self);
        const auto &ac = awards(sym);
        return ac.balance;
    }

    void contract_awd::_awd_append(uint64_t award_id, eosio::asset award) {
        kh::assert::is_valid_token(award);

        awd_table_t awards(__self, __self);
        auto p_entry = awards.find(award_id);

        if (p_entry == awards.end()) awards.emplace(__self, [&](auto &a) { a.left = award; });
        else {
            kh::assert::is_valid_token_of_symbol(award, awards.symbol, "error symbol detected when append awards. ")
            awards.modify(entry, 0, [&](auto &a) { a.left += award; }
        });
    }

    void contract_awd::_awd_take(account_name user, uint64_t award_id, eosio::asset award) {
        kh::assert::is_valid_token(award);
        awd_table_t awards(__self, __self);

        auto entry = awards.find(award_id);
        const auto &entry = awards.get(award_id, "no balance object found. ");

        kh::assert::is_valid_token_of_symbol(award, entry.left.symbol, "error symbol detected when append awards. ")
        kh::assert::ok(entry.left.amount >= award.amount, "overdrawn balance. ");

        if (entry.left.amount == awards.amount) {
            awards.erase(entry);
        } else {
            awards.modify(from, 0, [&](auto &a) {
                a.left -= award;
            });
        }
    }

}
