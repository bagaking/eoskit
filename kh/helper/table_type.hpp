#pragma once

namespace kh {

    namespace helper {

        //@abi table maker i64
        struct [[eosio::table]] single_val_item_uint32_t {
            account_name key;
            uint32_t val;

            uint64_t primary_key() const {
                return key;
            }

            EOSLIB_SERIALIZE(single_val_item_uint32_t, (key)(val)
            )
        };

        //@abi table maker i64
        struct [[eosio::table]] single_val_item_string_t {
            account_name key;
            std::string val;

            uint64_t primary_key() const {
                return key;
            }

            EOSLIB_SERIALIZE(single_val_item_string_t, (key)(val)
            )
        };

        //@abi table maker i64
        struct [[eosio::table]] single_val_item_asset_t {
            account_name key;
            eosio::asset val;

            uint64_t primary_key() const {
                return key;
            }

            EOSLIB_SERIALIZE(single_val_item_asset_t, (key)(val)
            )
        };

        //@abi table maker i64
        struct [[eosio::table]] field_all_in_one_t {
            account_name key;

            uint32_t vint;
            std::string vstr;
            account_name vaname;
            eosio::asset vasset;

            uint64_t primary_key() const {
                return key;
            }

            EOSLIB_SERIALIZE(field_all_in_one_t, (key)(vint)(vstr)(vaname)(vasset)
            )
        };

        //@abi table maker i64
        struct [[eosio::table]] res_t {
            eosio::asset balance;

            uint64_t primary_key() const { return balance.symbol.name(); }

            EOSLIB_SERIALIZE(res_t, (balance)
            )
        };

        //@abi table maker i64
        struct [[eosio::table]] item_t {
            uint64_t cid; //id with catalog standard <catalog:2, catalog_id:6> .e.p 11000001
            uint64_t count;

            uint64_t primary_key() const { return cid; }

            EOSLIB_SERIALIZE(item_t, (cid)(count)
            )
        };
    }
}



