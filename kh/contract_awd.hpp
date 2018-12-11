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

        EOSLIB_SERIALIZE(award_t, (key)(left)
        )
    };

    class contract_awd {
    public:
        contract_awd(const account_name self) : __self(self) {}

        [[eosio::action]] void awdreceipt(account_name user, uint64_t award_id, eosio::asset award, std::string memo) {
            require_auth(__self);
            require_recipient(user);
        }

        inline eosio::asset _get_awards_left(uint64_t award_id) const;

    private:
        account_name __self;
        typedef eosio::multi_index<N(awd.info), award_t> awd_table_t;

    protected:
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
        void _awd_take(account_name user, uint64_t award_id, eosio::asset award, std::string memo);
    }; // namespace kh

    eosio::asset contract_awd::_get_awards_left(uint64_t award_id) const {
        awd_table_t awards(__self, __self);
        const auto &ac = awards.get(award_id);
        return ac.left;
    }

    void contract_awd::_awd_append(uint64_t award_id, eosio::asset award) {
        kh::assert::is_valid_token(award);

        awd_table_t awards(__self, __self);
        auto p_entry = awards.find(award_id);

        if (p_entry == awards.end()) {
            awards.emplace(__self, [&](auto &a) {
                a.key = award_id;
                a.left = award;
            });
        } else {
            kh::assert::is_valid_token_of_symbol(award, p_entry->left.symbol);
            awards.modify(p_entry, 0, [&](auto &a) { a.left += award; });
        };
    }

    void contract_awd::_awd_take(account_name user, uint64_t award_id, eosio::asset quantity, std::string memo) {
        kh::assert::is_valid_token(quantity);
        awd_table_t awards(__self, __self);

        const auto &entry = awards.get(award_id, "no balance object found. ");

        kh::assert::is_valid_token_of_symbol(quantity, entry.left.symbol);
        kh::assert::ok(entry.left.amount >= quantity.amount, "overdrawn balance. ");

        if (entry.left.amount == quantity.amount) {
            awards.erase(entry);
        } else {
            awards.modify(entry, 0, [&](auto &a) {
                a.left -= quantity;
            });
        }

        // send receipt to seller, buyer
        kh::utils::call(__self, __self, N(awdreceipt),
                        make_tuple(user, award_id, quantity, memo)
        );
    }

}


#define KH_EXPORT_AWD (awdreceipt)