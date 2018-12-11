#pragma once

#include "./utils/assert.hpp"
#include "./utils/util.hpp"

#include "./helper/table_type.hpp"

namespace kh {

    class contract_item {
    public:
        contract_item(const account_name self) : __self(self) {}

        [[eosio::action]] void itemissue(account_name user,
                                         eosio::asset quantity,
                                         std::string memo);

        [[eosio::action]] void itemburn(account_name user,
                                        eosio::asset quantity,
                                        std::string memo);

        [[eosio::action]] void itemconv(account_name from,      // owner of src
                                        uint64_t src_cid,       // cid of src item
                                        uint64_t src_count,     // cid of src item
                                        account_name to,        // owner of src
                                        uint64_t dst_cid,       // cid of des item
                                        uint64_t dst_count,
                                        std::string memo);     // cid of des item

        [[eosio::action]] void itemreceipt(account_name user, eosio::asset from, eosio::asset to, std::string memo) {
            require_auth(__self);
            require_recipient(user);
        }

        inline uint64_t _get_item_count(account_name user, uint64_t cid) const;

    public:
        account_name __self;

        inline void _send_receipt(account_name user,
                                  uint64_t cid,
                                  uint64_t origin_count,
                                  uint64_t final_count,
                                  std::string memo) const;

    private:

        typedef eosio::multi_index<N(res.items), helper::item_t> item_table_t;

        void _item_sub(account_name user, uint64_t cid, uint64_t delta, std::string memo);

        void _item_add(account_name user, uint64_t cid, uint64_t delta, std::string memo);
    }; // namespace kh

    uint64_t contract_item::_get_item_count(account_name user, uint64_t cid) const {
        item_table_t items(__self, user);
        const auto &item = items.get(cid);
        return item.count;
    }

    void contract_item::_send_receipt(account_name user,
                                      uint64_t cid,
                                      uint64_t origin_count,
                                      uint64_t final_count,
                                      std::string memo) const {
        kh::utils::call(__self, __self, N(itemreceipt), make_tuple(user, cid, origin_count, final_count, memo));
    }

    void contract_item::_item_sub(account_name user, uint64_t cid, uint64_t delta, std::string memo) {
        item_table_t items(__self, user);
        const auto &item = items.get(cid, "item not found");
        kh::assert::ok(item.count >= delta, "no enough item");

        uint64_t origin_count = item.count;
        uint64_t final_count = 0;
        if (origin_count == final_count) {
            items.erase(item);
        } else {
            items.modify(item, 0, [&](auto &a) {
                final_count = (a.count -= delta);
            });
        }
        _send_receipt(user, cid, origin_count, final_count, memo);
    }

    void contract_item::_item_add(account_name user, uint64_t cid, uint64_t delta, std::string memo) {
        item_table_t items(__self, user);
        auto p_item = items.find(cid);

        uint64_t origin_count = 0;
        uint64_t final_count = delta;

        if (p_item == items.end()) {
            items.emplace(__self, [&](auto &a) { a.count = delta; });
        } else {
            items.modify(p_item, 0, [&](auto &a) {
                origin_count = a.count;
                final_count = (a.count += delta);
            });
        }
        _send_receipt(user, cid, origin_count, final_count, memo);
    }

    void contract_item::itemconv(account_name from,        // owner of src
                                 uint64_t src_cid,         // cid of src item
                                 uint64_t src_count,       // cid of src item
                                 account_name to,          // owner of src
                                 uint64_t dst_cid,         // cid of des item
                                 uint64_t dst_count,
                                 std::string memo) {

        // todo: implement this
    }

    void contract_item::itemissue(account_name user,
                                  eosio::asset quantity,
                                  std::string memo) {
        // todo: implement this
    }

    void contract_item::itemburn(account_name user,
                                 eosio::asset quantity,
                                 std::string memo) {
        // todo: implement this
    }

    void contract_item::itemtransfer(account_name from,
                                     account_name to,
                                     eosio::asset quantity,
                                     std::string memo) {
        // todo: implement this
    }

}

#define KH_EXPORT_ITEM (itemconv)(itemreceipt)

/*
 * example
 *
 * todo: fill this
 *
 */