#pragma once

#include "./utils/assert.hpp"
#include "./utils/util.hpp"

#include "./helper/table_type.hpp"

namespace kh {

    class contract_itm {
    public:
        contract_itm(const account_name self) : __self(self) {}

        [[eosio::action]] void itemissue(account_name user,
                                         uint64_t cid,
                                         uint64_t count,
                                         std::string memo);

        [[eosio::action]] void itemburn(account_name user,
                                        uint64_t cid,
                                        uint64_t count,
                                        std::string memo);

        [[eosio::action]] void itemreset(account_name user,
                                         uint64_t cid,
                                         std::string memo);


        [[eosio::action]] void itemconv(account_name from,      // owner of src
                                        uint64_t src_cid,       // cid of src item
                                        uint64_t src_count,     // cid of src item
                                        account_name to,        // owner of src
                                        uint64_t dst_cid,       // cid of des item
                                        uint64_t dst_count,
                                        std::string memo);     // cid of des item

        [[eosio::action]] void itemreceipt(account_name user, uint64_t cid, uint64_t origin_count, uint64_t final_count, std::string memo) {
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

        typedef eosio::multi_index<N(itm.accounts), helper::item_t> item_table_t;

        void _item_sub(account_name user, uint64_t cid, uint64_t delta, std::string memo);

        void _item_add(account_name user, uint64_t cid, uint64_t delta, std::string memo);
    }; // namespace kh

    uint64_t contract_itm::_get_item_count(account_name user, uint64_t cid) const {
        item_table_t items(__self, user);
        const auto &item = items.get(cid);
        return item.count;
    }

    void contract_itm::_send_receipt(account_name user,
                                      uint64_t cid,
                                      uint64_t origin_count,
                                      uint64_t final_count,
                                      std::string memo) const {
        kh::utils::call(__self, __self, N(itemreceipt), make_tuple(user, cid, origin_count, final_count, memo));
    }

    void contract_itm::_item_sub(account_name user, uint64_t cid, uint64_t delta, std::string memo) {
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

    void contract_itm::_item_add(account_name user, uint64_t cid, uint64_t delta, std::string memo) {
        item_table_t items(__self, user);
        auto p_item = items.find(cid);

        uint64_t origin_count = 0;
        uint64_t final_count = delta;

        if (p_item == items.end()) {
            items.emplace(__self, [&](auto &a) { a.count = delta; a.cid = cid;});
        } else {
            items.modify(p_item, 0, [&](auto &a) {
                origin_count = a.count;
                final_count = (a.count += delta);
            });
        }
        _send_receipt(user, cid, origin_count, final_count, memo);
    }

    void contract_itm::itemconv(account_name from,        // owner of src
                                 uint64_t src_cid,         // cid of src item
                                 uint64_t src_count,       // cid of src item
                                 account_name to,          // owner of src
                                 uint64_t dst_cid,         // cid of des item
                                 uint64_t dst_count,
                                 std::string memo) {
        require_auth(__self);
        kh::assert::ok(is_account(from), "from account does not exist");
        kh::assert::ok(is_account(to), "to account does not exist");

        require_recipient(from);
        require_recipient(to);

        _item_sub(from,src_cid,src_count,memo);
        _item_add(to,dst_cid,dst_count,memo);
    }

    void contract_itm::itemissue(account_name user,
                                  uint64_t cid,
                                  uint64_t count,
                                  std::string memo) {
        require_auth(__self);
        kh::assert::ok(memo.size() <= 256,"memo has more than 256 bytes");

        _item_add(user,cid,count,memo);
    }

    void contract_itm::itemburn(account_name user,
                                 uint64_t cid,
                                 uint64_t count,
                                 std::string memo) {
        require_auth(__self);
        kh::assert::ok(memo.size() <= 256,"memo has more than 256 bytes");

        _item_sub(user,cid,count,memo);
    }

    void contract_itm::itemreset(account_name user,
                                  uint64_t cid,
                                  std::string memo) {
        require_auth(__self);
        kh::assert::ok(memo.size() <= 256,"memo has more than 256 bytes");
        uint64_t count = _get_item_count(user,cid);
        _item_sub(user,cid,count,memo);
    }

}

#define KH_EXPORT_ITM (itemreceipt)(itemissue)(itemburn)(itemreset)(itemconv)

/*
 * example
 *
 *  cleos get table sco bob itm.accounts
 *
 *  cleos push action sco itemissue '["bob","1","50","memo"]' -p sco@active
 *  cleos push action sco itemburn '["bob","2","10","memo"]' -p sco@active
 *  cleos push action sco itemreset '["bob","2","memo"]' -p sco@active
 *  cleos push action sco itemconv '["bob","2","30","bob","3","40","memo"]' -p sco@active
 *
 */